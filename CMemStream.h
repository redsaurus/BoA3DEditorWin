#pragma once

const size_t kMsGrow = 0x100000;		// default grow factor

class CMemStream
{
public:
	enum EMsFrom {		// position indicater for SetMarker()
		eMsStart = 1,
		eMsEnd,
		eMsMarker,
	};

	CMemStream(void);
	virtual ~CMemStream(void);

	int SetLength( size_t inLength = kMsGrow );
	void SetGrow( size_t inGrow = kMsGrow );
	char* GetBuffer( void );
	char* GetCurrentPtr( void );

	void SetMarker( size_t inOffset, EMsFrom inFromWhere = eMsStart );
	int GetBytes( size_t inByteCount, void* outDst );
	int PutBytes( size_t inByteCount, const void* inSrc );
	int TransferBytes( size_t inBytecount, CMemStream* outDst );

	int ReadFromFile( char* filePath );
	int WriteToFile( char* filePath );

protected:
	char* mDataBuf;
	size_t mBufSize;
	size_t mGrow;
	size_t mLength;
	size_t mMarker;
};
