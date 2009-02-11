#include "StdAfx.h"
#include "CMemStream.h"

CMemStream::CMemStream(void)
{
	mDataBuf = NULL;
	mBufSize = 0;
	mGrow = kMsGrow;
	mLength = 0;
	mMarker = 0;
}

CMemStream::~CMemStream(void)
{
	free( mDataBuf );
	mDataBuf = NULL;
}

int CMemStream::SetLength( size_t inLength )
{
	if (mDataBuf == NULL) {				// Allocate new buffer for data
		if ( mGrow ) {
			mDataBuf = (char *)malloc( mGrow );
			mBufSize = mGrow;
		} else {
			mDataBuf = (char *)malloc( inLength );
			mBufSize = inLength;
		}
	} else {							// Resize existing buffer
		if ( mGrow ) {
			size_t newSize = ( inLength / mGrow + 1 ) * mGrow;
			if ( newSize != mBufSize ) {
				mDataBuf = (char *)realloc( mDataBuf, newSize );
				mBufSize = newSize;
			}
		} else {
			mDataBuf = (char *)realloc( mDataBuf, inLength );
			mBufSize = inLength;
		}
	}
	if ( mDataBuf == NULL ) {
		mBufSize = 0;
		mLength = 0;
		mMarker = 0;
		return -1;
	}

	size_t oldLength = mLength;
	mLength = inLength;
	if ( oldLength > inLength ) {
		SetMarker(mMarker, eMsStart);
	}
	return 0;
}

void CMemStream::SetGrow( size_t inGrow )
{
	mGrow = inGrow;
}

char* CMemStream::GetBuffer( void )
{
	return mDataBuf;
}

char* CMemStream::GetCurrentPtr( void )
{
	return mDataBuf + mMarker;
}

void CMemStream::SetMarker( size_t inOffset, EMsFrom inFromWhere )
{
	size_t	newMarker = mMarker;

	switch (inFromWhere) {
	
		case eMsStart:
			newMarker = inOffset;
			break;
			
		case eMsEnd:
			newMarker = mLength - inOffset;
			break;
			
		case eMsMarker:
			newMarker += inOffset;
			break;
	}
	
	if (newMarker < 0) {
		newMarker = 0;
	} else if (newMarker > mLength) {
		newMarker = mLength;
	}

	mMarker = newMarker;
}

int CMemStream::GetBytes( size_t inByteCount, void* outDst )
{
	if (mDataBuf == NULL)					return -1;
	if ((mMarker + inByteCount) > mLength)	return -1;
	memcpy(outDst, mDataBuf + mMarker, inByteCount);
	SetMarker(inByteCount, eMsMarker);
	return 0;
}

int CMemStream::PutBytes( size_t inByteCount, const void* inSrc )
{
	size_t endOfWrite = mMarker + inByteCount;
	
	if (endOfWrite > mLength) {			// Need to grow the buffer
		if ( SetLength( endOfWrite ) != 0 ) 
			return -1;
	}
	if (inByteCount > 0) {
		memcpy( mDataBuf + mMarker, inSrc, inByteCount );
		SetMarker(inByteCount, eMsMarker);
	}
	return 0;
}

int CMemStream::TransferBytes( size_t inByteCount, CMemStream * outDst )
{
	if (mDataBuf == NULL)					return -1;
	if ((mMarker + inByteCount) > mLength)	return -1;
	if ( outDst->PutBytes( inByteCount, mDataBuf + mMarker ) != 0 )	return -1;
	SetMarker(inByteCount, eMsMarker);
	return 0;
}

int CMemStream::ReadFromFile( char * filePath )
{
	FILE* fp = NULL;
	size_t readLen;
	int result = -1;
	try {
		if ( (fp = fopen(filePath, "rb") ) == NULL )	throw 0;
		if ( fseek( fp, 0, SEEK_END ) != 0)				throw 0;
			 readLen = ftell( fp );
		if ( fseek( fp, 0, SEEK_SET ) != 0)				throw 0;
		if ( SetLength( readLen ) != 0 )				throw 0;
		if ( fread( mDataBuf, readLen, 1, fp ) != 1 )	throw 0;
		result = 0;
	}
	catch(...)
	{
	}
	if ( fp ) fclose( fp );
	SetMarker(0, eMsStart);
	return result;
}

int CMemStream::WriteToFile( char * filePath )
{
	FILE* fp = NULL;
	int result = -1;
	try {
		if ( (fp = fopen(filePath, "wb") ) == NULL )	throw 0;
		if ( fwrite( mDataBuf, mLength, 1, fp ) != 1 )	throw 0;
		result = 0;
	}
	catch(...)
	{
	}
	if ( fp ) fclose( fp );
	return result;
}
