#pragma once
#include <cstdint>
#include <algorithm>

#define LittleDWord( val )            ( val )

class bf_read {
public:
	const char* m_pDebugName;
	bool m_bOverflow;
	int m_nDataBits;
	unsigned int m_nDataBytes;
	unsigned int m_nInBufWord;
	int m_nBitsAvail;
	const unsigned int* m_pDataIn;
	const unsigned int* m_pBufferEnd;
	const unsigned int* m_pData;

	bf_read() = default;

	bf_read(const void* pData, int nBytes, int nBits = -1) {
		StartReading(pData, nBytes, 0, nBits);
	}

	void StartReading(const void* pData, int nBytes, int iStartBit, int nBits) {
		// Make sure it's dword aligned and padded.
		m_pData = (uint32_t*)pData;
		m_pDataIn = m_pData;
		m_nDataBytes = nBytes;

		if (nBits == -1) {
			m_nDataBits = nBytes << 3;
		}
		else {
			m_nDataBits = nBits;
		}
		m_bOverflow = false;
		m_pBufferEnd = reinterpret_cast<uint32_t const*>(reinterpret_cast<uint8_t const*>(m_pData) + nBytes);
		if (m_pData)
			Seek(iStartBit);
	}

	bool Seek(int nPosition) {
		bool bSucc = true;
		if (nPosition < 0 || nPosition > m_nDataBits) {
			m_bOverflow = true;
			bSucc = false;
			nPosition = m_nDataBits;
		}
		int nHead = m_nDataBytes & 3; // non-multiple-of-4 bytes at head of buffer. We put the "round off"
		// at the head to make reading and detecting the end efficient.

		int nByteOfs = nPosition / 8;
		if ((m_nDataBytes < 4) || (nHead && (nByteOfs < nHead))) {
			// partial first dword
			uint8_t const* pPartial = (uint8_t const*)m_pData;
			if (m_pData) {
				m_nInBufWord = *(pPartial++);
				if (nHead > 1)
					m_nInBufWord |= (*pPartial++) << 8;
				if (nHead > 2)
					m_nInBufWord |= (*pPartial++) << 16;
			}
			m_pDataIn = (uint32_t const*)pPartial;
			m_nInBufWord >>= (nPosition & 31);
			m_nBitsAvail = (nHead << 3) - (nPosition & 31);
		}
		else {
			int nAdjPosition = nPosition - (nHead << 3);
			m_pDataIn = reinterpret_cast<uint32_t const*>(
				reinterpret_cast<uint8_t const*>(m_pData) + ((nAdjPosition / 32) << 2) + nHead);
			if (m_pData) {
				m_nBitsAvail = 32;
				GrabNextDWord();
			}
			else {
				m_nInBufWord = 0;
				m_nBitsAvail = 1;
			}
			m_nInBufWord >>= (nAdjPosition & 31);
			m_nBitsAvail = (std::min)(m_nBitsAvail, 32 - (nAdjPosition & 31)); // in case grabnextdword overflowed
		}
		return bSucc;
	}

	__forceinline void GrabNextDWord(bool bOverFlowImmediately = false) {
		if (m_pDataIn == m_pBufferEnd) {
			m_nBitsAvail = 1; // so that next read will run out of words
			m_nInBufWord = 0;
			m_pDataIn++; // so seek count increments like old
			if (bOverFlowImmediately)
				m_bOverflow = true;
		}
		else if (m_pDataIn > m_pBufferEnd) {
			m_bOverflow = true;
			m_nInBufWord = 0;
		}
		else {
			m_nInBufWord = LittleDWord(*(m_pDataIn++));
		}
	}
};

class bf_write {
public:
	unsigned char* m_pData;
	int m_nDataBytes;
	int m_nDataBits;
	int m_iCurBit;
	bool m_bOverflow;
	bool m_bAssertOnOverflow;
	const char* m_pDebugName;

	void StartWriting(void* pData, int nBytes, int iStartBit = 0, int nBits = -1) {
		// Make sure it's dword aligned and padded.
		// The writing code will overrun the end of the buffer if it isn't dword aligned, so truncate to force alignment
		nBytes &= ~3;

		m_pData = (unsigned char*)pData;
		m_nDataBytes = nBytes;

		if (nBits == -1) {
			m_nDataBits = nBytes << 3;
		}
		else {
			m_nDataBits = nBits;
		}

		m_iCurBit = iStartBit;
		m_bOverflow = false;
	}

	bf_write() {
		m_pData = NULL;
		m_nDataBytes = 0;
		m_nDataBits = -1; // set to -1 so we generate overflow on any operation
		m_iCurBit = 0;
		m_bOverflow = false;
		m_bAssertOnOverflow = true;
		m_pDebugName = NULL;
	}

	// nMaxBits can be used as the number of bits in the buffer.
	// It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
	bf_write(void* pData, int nBytes, int nBits = -1) {
		m_bAssertOnOverflow = true;
		m_pDebugName = NULL;
		StartWriting(pData, nBytes, 0, nBits);
	}

	bf_write(const char* pDebugName, void* pData, int nBytes, int nBits = -1) {
		m_bAssertOnOverflow = true;
		m_pDebugName = pDebugName;
		StartWriting(pData, nBytes, 0, nBits);
	}
};

#define EVENT_DEBUG_ID_INIT 42 
#define EVENT_DEBUG_ID_SHUTDOWN 13  

class IGameEvent
{
public:
	virtual ~IGameEvent() = 0;
	virtual const char* GetName() const = 0;

	virtual bool            IsReliable() const = 0;
	virtual bool            IsLocal() const = 0;
	virtual bool            IsEmpty(const char* keyName = nullptr) = 0;

	virtual bool            GetBool(const char* keyName = nullptr, bool defaultValue = false) = 0;
	virtual int             GetInt(const char* keyName = nullptr, int defaultValue = 0) = 0;
	virtual uint64_t        GetUint64(const char* keyName = nullptr, unsigned long defaultValue = 0) = 0;
	virtual float           GetFloat(const char* keyName = nullptr, float defaultValue = 0.0f) = 0;
	virtual const char* GetString(const char* keyName = nullptr, const char* defaultValue = "") = 0;
	virtual const wchar_t* GetWString(const char* keyName, const wchar_t* defaultValue = L"") = 0;
	virtual const void* GetPointer(const char* keyName = nullptr, const void* defaultValue = nullptr) = 0;

	virtual void            SetBool(const char* keyName, bool value) = 0;
	virtual void            SetInt(const char* keyName, int value) = 0;
	virtual void            SetUint64(const char* keyName, unsigned long value) = 0;
	virtual void            SetFloat(const char* keyName, float value) = 0;
	virtual void            SetString(const char* keyName, const char* value) = 0;
	virtual void            SetWString(const char* keyName, const wchar_t* value) = 0;
	virtual void			  SetPointer(const char* keyName, const void* value) = 0;

	bool GetBool_(const char* keyName) {
		return GetBool(keyName);
	}

	int GetInt_(const char* keyName) {
		return GetInt(keyName);
	}

	uint64_t GetUint64_(const char* keyName) {
		return GetUint64(keyName);
	}

	float GetFloat_(const char* keyName) {
		return GetFloat(keyName);
	}

	const char* GetString_(const char* keyName) {
		return GetString(keyName);
	}

	const wchar_t* GetWString_(const char* keyName) {
		return GetWString(keyName);
	}
};

class IGameEventListener
{
public:
	virtual ~IGameEventListener(void) {}

	virtual void FireGameEvent(IGameEvent* event) = 0;
	virtual int  GetEventDebugID(void) = 0;

public:
	int m_iDebugId;
};

class IGameEventManager
{
public:
	virtual             ~IGameEventManager() = 0;
	virtual int         LoadEventsFromFile(const char* filename) = 0;
	virtual void        Reset() = 0;
	virtual bool        AddListener(IGameEventListener* listener, const char* name, bool bServerSide) = 0;
	virtual bool        FindListener(IGameEventListener* listener, const char* name) = 0;
	virtual int         RemoveListener(IGameEventListener* listener) = 0;
	virtual IGameEvent* CreateEvent(const char* name, bool bForce, unsigned int dwUnknown) = 0;
	virtual bool        FireEvent(IGameEvent* event, bool bDontBroadcast = false) = 0;
	virtual bool        FireEventClientSide(IGameEvent* event) = 0;
	virtual IGameEvent* DuplicateEvent(IGameEvent* event) = 0;
	virtual void        FreeEvent(IGameEvent* event) = 0;
	virtual bool        SerializeEvent(IGameEvent* event, bf_write* buf) = 0;
	virtual IGameEvent* UnserializeEvent(bf_read* buf) = 0;
};
