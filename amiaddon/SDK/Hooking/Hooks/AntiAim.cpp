#include "../Hooks.hpp"
#include "../../Classes/PlayerAnimState.hpp"
#include <cmath>
#include <cfloat>
#include <algorithm>

#pragma region decl_macros
#define MULTIPLAYER_BACKUP 150

#define FL_ONGROUND  ( 1 << 0 )
#define FL_DUCKING   ( 1 << 1 )

#define IN_ATTACK    ( 1 << 0 )
#define IN_JUMP      ( 1 << 1 )
#define IN_DUCK      ( 1 << 2 )
#define IN_FORWARD   ( 1 << 3 )
#define IN_BACK      ( 1 << 4 )
#define IN_USE       ( 1 << 5 )
#define IN_CANCEL    ( 1 << 6 )
#define IN_LEFT      ( 1 << 7 )
#define IN_RIGHT     ( 1 << 8 )
#define IN_MOVELEFT  ( 1 << 9 )
#define IN_MOVERIGHT ( 1 << 10 )
#define IN_ATTACK2   ( 1 << 11 )
#define IN_RUN       ( 1 << 12 )
#define IN_RELOAD    ( 1 << 13 )
#define IN_ALT1      ( 1 << 14 )
#define IN_ALT2      ( 1 << 15 )
#define IN_SCORE     ( 1 << 16 )
#define IN_SPEED     ( 1 << 17 )
#define IN_WALK      ( 1 << 18 )
#define IN_ZOOM      ( 1 << 19 )
#define IN_WEAPON1   ( 1 << 20 )
#define IN_WEAPON2   ( 1 << 21 )
#define IN_BULLRUSH  ( 1 << 22 )
#define IN_GRENADE1  ( 1 << 23 )
#define IN_GRENADE2  ( 1 << 24 )
#pragma endregion

// ─────────────────────────────────────────────────────────────────────────────
// LBY BREAK STATE  (ported from family_anti_aim reversal — AntiAimCtx fields)
// ─────────────────────────────────────────────────────────────────────────────
namespace LBYState
{
    // mirrors AntiAimCtx::break_phase  (0=backwards, 1=sell, 2=+/-90)
    static int   s_iBreakPhase = 0;
    // mirrors AntiAimCtx::break_state  (0=ready, 1+=breaking)
    static int   s_iBreakState = 0;
    // mirrors AntiAimCtx::stored_break
    static float s_flStoredBreak = 0.f;
    // mirrors AntiAimCtx::target_entity used as desync dir slot (0-7, -1=backwards)
    static int   s_iDesyncDir = -1;
    // mirrors original bFirstBreak
    static bool  s_bFirstBreak = false;
    // mirrors original flBodyUpdate (bookkeeping only)
    static float s_flBodyUpdate = 0.f;
}

// ─────────────────────────────────────────────────────────────────────────────
// HELPERS
// ─────────────────────────────────────────────────────────────────────────────

// sub_10057D50 — exact port: loop-based normalize, NOT fmod
static float NormalizeYaw(float yaw)
{
    while (yaw > 180.f) yaw -= 360.f;
    while (yaw < -180.f) yaw += 360.f;
    return yaw;
}

// AngleDiff: used in pre-flick footToBody computation
static float AngleDiff(float dest, float src)
{
    float delta = std::fmod(dest - src, 360.f);
    if (delta > 180.f) delta -= 360.f;
    else if (delta < -180.f) delta += 360.f;
    return delta;
}

// ApproachAngle: used in break-window delay branch
static float ApproachAngle(float target, float value, float speed)
{
    float delta = NormalizeYaw(target - value);
    if (delta > speed) return value + speed;
    else if (delta < -speed) return value - speed;
    return target;
}

// sub_10092490 — angle computation: base + delta, clamped to desyncMax.
// desyncMax mirrors ctx->desync_max (config slider, practical max 135 deg).
static float ComputeDesyncAngle(float base, float delta, float desyncMax = 135.f)
{
    if (std::fabs(delta) > desyncMax)
        delta = (delta > 0.f) ? desyncMax : -desyncMax;
    return NormalizeYaw(base + delta);
}

// sub_100930B0 — select angle for choke phase.
//   phase 0 = backwards (targetBody+180)
//   phase 1 = real/sell (realYaw)
//   phase 2 = preferred +/-90 side relative to realYaw
static float PhaseAngle(float targetBody, float realYaw, int phase)
{
    switch (phase)
    {
    case 0:
        return NormalizeYaw(targetBody + 180.f);
    case 1:
        return realYaw;
    case 2:
    {
        float left = NormalizeYaw(targetBody - 90.f);
        float right = NormalizeYaw(targetBody + 90.f);
        float dl = std::fabs(NormalizeYaw(left - realYaw));
        float dr = std::fabs(NormalizeYaw(right - realYaw));
        // Return the preferred (smaller delta) side.
        // The reversal commits less-preferred first then preferred;
        // on the single-angle we write the preferred side.
        return (dl < dr) ? left : right;
    }
    default:
        return NormalizeYaw(targetBody + 180.f);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// AIR / WALK ANTI-AIM  (sub_100919F0, mode == 2)
//
// Fixes applied:
//  1. Safety loop: removed the check against (flViewYaw+180) — checking the
//     chosen fake against itself as a reference would always fire for the
//     backwards direction and spin the angle into garbage.  We only check
//     against LBY / foot-yaw / vel-yaw as in the original air block.
//  2. desyncDir advance: moved to AFTER writing the angle (not before) to
//     match the reversal's order:  write angle → advance dir on send tick.
//  3. desyncDir init on first use: -1 maps to dirMod==0 (backwards) correctly.
// ─────────────────────────────────────────────────────────────────────────────
static void HandleAirAntiAim(CCSGOPlayerAnimState* pState,
    C_CSPlayer* pLocal,
    CUserCmd* pCmd,
    float                 flViewYaw)
{
    pCmd->viewangles.x = 89.f;

    Vector vel = pLocal->m_vecVelocity();
    float flVelYaw = std::atan2(vel.y, vel.x) * (180.f / 3.14159265358979323846f);
    float flLBY = pLocal->m_flLowerBodyYawTarget();
    float flFootYaw = pState->m_flFootYaw;

    // Reversal desync direction table (sub_100919F0 switch % 8):
    //   case 0,1  -> backwards (+180)      break_phase = 0
    //   case 2,3  -> sell real yaw          break_phase = 1
    //   case 4,5  -> left  (-desyncMax)    break_phase = 2
    //   case 6,7  -> right (+desyncMax)    break_phase = 2
    constexpr float desyncMax = 58.f;
    int   dir = LBYState::s_iDesyncDir;
    int   dirMod = (dir < 0) ? 0 : (dir % 8);
    float result;

    if (dirMod <= 1)
    {
        result = NormalizeYaw(flViewYaw + 180.f);
        LBYState::s_iBreakPhase = 0;
    }
    else if (dirMod <= 3)
    {
        result = flViewYaw; // sell angle — mirrors ctx->real_body_yaw
        LBYState::s_iBreakPhase = 1;
    }
    else if (dirMod <= 5)
    {
        result = NormalizeYaw(flViewYaw - desyncMax);
        LBYState::s_iBreakPhase = 2;
    }
    else
    {
        result = NormalizeYaw(flViewYaw + desyncMax);
        LBYState::s_iBreakPhase = 2;
    }

    // Safety loop: jitter until angle is >45 deg away from LBY / foot / vel yaw.
    // Fix: do NOT compare result vs (flViewYaw+180) — that fires unconditionally
    // on the backwards direction and makes the loop spin pointlessly.
    int safety = 0;
    while ((std::fabs(NormalizeYaw(result - flLBY)) <= 45.f
        || std::fabs(NormalizeYaw(result - flFootYaw)) <= 45.f
        || std::fabs(NormalizeYaw(result - flVelYaw)) <= 45.f)
        && safety++ < 72)
    {
        result += (g_pClientState->m_last_outgoing_command % 2 == 0) ? 10.f : -10.f;
    }

    pCmd->viewangles.y = result;

    // Advance desync direction on send ticks.
    // sub_100919F0: target_entity = (desync_dir + 1) % 8
    // Approximation: m_choked_commands == 0 means this is a send tick.
    if (g_pClientState->m_choked_commands == 0)
        LBYState::s_iDesyncDir = (dir < 0) ? 1 : ((dir + 1) % 8);

    LBYState::s_flBodyUpdate = Family::FamilyCurTime + 0.22f;
    LBYState::s_bFirstBreak = true;
}

// ─────────────────────────────────────────────────────────────────────────────
// STANDING ANTI-AIM  (sub_10090E20, mode == 1)
//
// Execution order matches reversal exactly:
//
//  PHASE 1 — LBY break-window  (curtime > m_flTimeToAlignLowerBody)
//    break_state==0 && shouldDelay && footDelta>35  ->  ApproachAngle, prime resolver
//    else                                           ->  commit backwards, reset break_state
//
//  PHASE 2 — Pre-flick multi-choke  (curtime > TimeToAlign - 4 ticks)
//    s_iBreakPhase 0 -> backwards (+180)
//    s_iBreakPhase 1 -> sell real yaw
//    s_iBreakPhase 2 -> best +/-90 side  (less-preferred first per sub_10093710)
//    Phase advances on each send tick (0->1->2->0).
//
//  PHASE 3 — LBY maintenance  (TimeToAlign-1.1s == m_flLastUpdateTime)
//    sub_10097520 / sub_10096260:
//      |diff| < 26 -> sideways (+90)
//      |diff| < 58 -> backwards (+180)
//    Overrides only the matching range; foot-delta angle is the baseline.
//
//  FALLBACK — flLBY + (bFirstBreak ? 0 : 90)
//
// Fixes applied:
//  A. breakAngle was computed but the raw ApproachAngle immediately overwrote
//     viewangles — the approach angle IS the intended output during the prime
//     tick; breakAngle is now stored in s_flStoredBreak only.
//  B. Phase ordering was wrong: pre-flick (phase 2) was evaluated before the
//     break-window (phase 1).  Fixed to match the reversal's execution order.
//  C. Maintenance override logic was applying adjAngle unconditionally for all
//     absDiff<58, collapsing sideways and backwards into one branch.  Now
//     correctly checks absDiff<26 for sideways and 26<=absDiff<58 for backwards.
// ─────────────────────────────────────────────────────────────────────────────
static void HandleStandAntiAim(CCSGOPlayerAnimState* pState,
    C_CSPlayer* pLocal,
    CUserCmd* pCmd,
    float                 flViewYaw)
{
    pCmd->viewangles.x = 89.f;

    const float flWantedBodyYaw = NormalizeYaw(flViewYaw + 180.f);
    const float flLBY = pLocal->m_flLowerBodyYawTarget();
    const float flFootYaw = pState->m_flFootYaw;

    auto TICKS_TO_TIME = [](int t) -> float { return t * (1.f / 64.f); };

    // ── PHASE 1: LBY BREAK WINDOW ────────────────────────────────────────────
    if (Family::FamilyCurTime > pState->m_flTimeToAlignLowerBody)
    {
        float footDelta = NormalizeYaw(flWantedBodyYaw - flFootYaw);
        bool  shouldDelayTimer = (pState->m_flLastUpdateTime <= pState->m_flTimeToAlignLowerBody);

        if (shouldDelayTimer && std::fabs(footDelta) > 35.f
            && LBYState::s_iBreakState == 0)
        {
            // sub_10090E20: compute break_angle = sub_10092490(target_body, 180)
            // Write to stored_break; viewangle is the sideways approach.
            LBYState::s_flStoredBreak = ComputeDesyncAngle(flWantedBodyYaw, 180.f);
            pCmd->viewangles.y = ApproachAngle(flViewYaw + 90.f, flFootYaw, 30.f);

            // On send tick: break_state++, stored_break = 0
            if (g_pClientState->m_choked_commands == 0)
            {
                LBYState::s_iBreakState++;
                LBYState::s_flStoredBreak = 0.f;
            }

            LBYState::s_flBodyUpdate = Family::FamilyCurTime + 0.22f;
            LBYState::s_bFirstBreak = true;
            return;
        }

        // Server timer caught up — reset break_state on send tick
        if (g_pClientState->m_choked_commands == 0)
            LBYState::s_iBreakState = 0;

        // Commit backwards yaw (real desync angle post-break)
        pCmd->viewangles.y = flWantedBodyYaw;

        LBYState::s_flBodyUpdate = Family::FamilyCurTime + 0.22f;
        LBYState::s_bFirstBreak = true;
        return;
    }

    // ── PHASE 2: PRE-FLICK / MULTI-CHOKE BODY-FAKE ───────────────────────────
    {
        const float preFlickTime = pState->m_flTimeToAlignLowerBody - TICKS_TO_TIME(4);

        if (Family::FamilyCurTime > preFlickTime)
        {
            float bodyFakeAngle;

            switch (LBYState::s_iBreakPhase)
            {
            case 0:
                // Stage 0 (sub_10093710): backwards, target_body + 180
                bodyFakeAngle = NormalizeYaw(flWantedBodyYaw + 180.f);
                break;

            case 1:
                // Stage 1: sell real angle (ctx->real_body_yaw == flViewYaw)
                bodyFakeAngle = flViewYaw;
                break;

            case 2:
            {
                // Stage 2: +/-90, less-preferred first.
                // Reversal (sub_10093710): if dl>dr commit left first then right,
                // else commit right first then left.  We write the less-preferred
                // angle; next tick (after phase resets to 0) preferred is implied.
                float left = NormalizeYaw(flWantedBodyYaw - 90.f);
                float right = NormalizeYaw(flWantedBodyYaw + 90.f);
                float dl = std::fabs(NormalizeYaw(left - flViewYaw));
                float dr = std::fabs(NormalizeYaw(right - flViewYaw));
                bodyFakeAngle = (dl > dr) ? left : right; // less-preferred
                break;
            }

            default:
            {
                // Fallback jitter: footToBody / |footToBody| * 110
                float footToBody = AngleDiff(flWantedBodyYaw, flFootYaw);
                if (std::fabs(footToBody) <= 0.f)
                    footToBody = 1.f;
                float flickDir = footToBody / std::fabs(footToBody);
                bodyFakeAngle = NormalizeYaw(flWantedBodyYaw + flickDir * 110.f);
                break;
            }
            }

            pCmd->viewangles.y = bodyFakeAngle;

            // Advance phase on send tick (0->1->2->0)
            if (g_pClientState->m_choked_commands == 0)
                LBYState::s_iBreakPhase = (LBYState::s_iBreakPhase + 1) % 3;

            LBYState::s_flBodyUpdate = Family::FamilyCurTime + 0.22f;
            LBYState::s_bFirstBreak = true;
            return;
        }
    }

    // ── PHASE 3: LBY MAINTENANCE (sub_10097520 / sub_10096260) ───────────────
    if ((pState->m_flTimeToAlignLowerBody - 1.1f) == pState->m_flLastUpdateTime)
    {
        float yawDelta = NormalizeYaw(flLBY - flWantedBodyYaw);

        if (std::fabs(yawDelta) <= 58.f)
        {
            float predictedFootDelta = NormalizeYaw(flLBY - flFootYaw);

            // Baseline: directional +/-58 from LBY (original logic)
            float angle = (predictedFootDelta > 0.f)
                ? NormalizeYaw(flLBY + 58.f)
                : NormalizeYaw(flLBY - 58.f);

            // sub_10097520 maintenance override (mode_flag branching):
            //   Fix: was applying both branches under a single absDiff<58 check.
            //   Must check absDiff<26 first (sideways) then 26<=absDiff<58 (backwards).
            float absDiff = std::fabs(yawDelta);
            if (absDiff < 26.f)
                angle = NormalizeYaw(flWantedBodyYaw + 90.f);  // sideways (mode_flag 1)
            else if (absDiff < 58.f)
                angle = NormalizeYaw(flViewYaw + 180.f);       // backwards (mode_flag 2)
            // else: absDiff >= 58 -- no override, keep foot-delta baseline

            pCmd->viewangles.y = angle;

            LBYState::s_flBodyUpdate = Family::FamilyCurTime + 0.22f;
            LBYState::s_bFirstBreak = true;
            return;
        }
    }

    // ── FALLBACK: BASE LBY + firstBreak OFFSET ───────────────────────────────
    pCmd->viewangles.y = flLBY + (LBYState::s_bFirstBreak ? 0.f : 90.f);

    LBYState::s_flBodyUpdate = Family::FamilyCurTime + 0.22f;
    LBYState::s_bFirstBreak = true;
}

// ─────────────────────────────────────────────────────────────────────────────
// MAIN HOOK  (hkAntiAim)
// ─────────────────────────────────────────────────────────────────────────────
char* __fastcall Hooked::hkAntiAim(int32_t* g_hvh)
{
    auto pLocal = C_CSPlayer::GetLocalPlayer();
    if (!pLocal || pLocal->IsDead())
        return oAntiAim(g_hvh);

    CCSGOPlayerAnimState* pState = pLocal->m_PlayerAnimState();
    if (!pState)
        return oAntiAim(g_hvh);

    // sub_10093FC0: freeze / freeze-period gate
    bool bFrozen = pLocal->m_fFlags() & 0x40 || g_pGameRules->m_bFreezePeriod();
    if (bFrozen)
        return oAntiAim(g_hvh);

    CUserCmd* pCmd = Hooked::pCurrentCmd;
    if (!pCmd)
        return oAntiAim(g_hvh);

    // ── EARLY EXIT (sub_10093FC0 / sub_10094470) ─────────────────────────────
    if (Family::bGameOver || Family::bRoundEnd
        || (((pCmd->forwardmove || pCmd->sidemove)
            || pLocal->m_vecVelocity().Length2D() > 0.1f)
            && (pLocal->m_MoveType() == 9))
        || pLocal->m_MoveType() == 8)
    {
        return oAntiAim(g_hvh);
    }

    if (pCmd->buttons & IN_USE)
        return oAntiAim(g_hvh);

    // ── READ CMD YAW (sub_1008D3E0 "READ CMD YAW" block) ─────────────────────
    // Capture BEFORE oAntiAim modifies viewangles — mirrors ctx->target_eye_yaw
    // / ctx->real_yaw which are set from the raw cmd yaw before any dispatch.
    const float flViewYaw = pCmd->viewangles.y;
    bool        bInAir = !(pLocal->m_fFlags() & FL_ONGROUND);

    // Call original for pitch and manual-direction baseline.
    char* ret = nullptr;
    oAntiAim(g_hvh);

    // ── DISPATCH ─────────────────────────────────────────────────────────────

    if (bInAir && pLocal->m_vecVelocity().Length2D() > 0.1f)
    {
        // sub_100919F0 / mode 2
        HandleAirAntiAim(pState, pLocal, pCmd, flViewYaw);
        // post-dispatch normalize (sub_10057D50 at LABEL_80)
        pCmd->viewangles.y = NormalizeYaw(pCmd->viewangles.y);
        return ret;
    }

    // Moving on ground — original already handled; update bookkeeping only.
    if (pLocal->m_vecVelocity().Length2D() > 0.1f)
    {
        LBYState::s_flBodyUpdate = Family::FamilyCurTime + 0.22f;
        LBYState::s_bFirstBreak = true;
        return ret;
    }

    // sub_10090E20 / mode 1
    HandleStandAntiAim(pState, pLocal, pCmd, flViewYaw);

    // post-dispatch normalize (sub_10057D50 at LABEL_80)
    pCmd->viewangles.y = NormalizeYaw(pCmd->viewangles.y);

    // Normalize stored_break (reversal STAND-mode post-processing)
    if (LBYState::s_flStoredBreak != 0.f)
        LBYState::s_flStoredBreak = NormalizeYaw(LBYState::s_flStoredBreak);

    return ret;
}