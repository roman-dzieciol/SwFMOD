/* ============================================================================
	SwFMODUtil.h:
	Copyright 2007 Roman Switch` Dzieciol. All Rights Reserved.
===============================================================================
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
============================================================================ */
#ifndef SWFMODUTIL_H
#define SWFMODUTIL_H



/* ----------------------------------------------------------------------------
	Helpers
---------------------------------------------------------------------------- */

inline bool HasFlag( INT val, INT flag ) { return (val & flag) == flag; }

template<class A, class B> inline void TSwOverride( A property, B override )
{
	if( override != -1 )
	{
		property = static_cast<A>(override);
	}
}


inline UBOOL SwVerifyFloat(FLOAT f)
{
	return !appIsNan(f);
}


template<class T> inline void TSwSortMinMax( T& vmin, T& vmax )
{
	if( vmin > vmax )
	{
		T temp = vmax;
		vmax = vmin;
		vmin = temp;
	}
}

inline void SwDebugLog( EName Type, FString str, const TCHAR* func=NULL, const char* file=NULL, DWORD line=0, UBOOL bTime=0 )
{
	FString s;

	if( bTime )
	{
		s += FString::Printf( TEXT("%s"), *SwTimeStr() );
	}

	if( func )
	{
		s += s ? TEXT(" :: ") : TEXT("");
		s += FString::Printf( TEXT("%s"), func );
	}

	if( file )
	{
		s += s ? TEXT(" ") : TEXT("");
		s += FString::Printf( TEXT("%s:%u"), appFromAnsi(file), line );
	}

	s += s ? TEXT(" :: ") : TEXT("");
	GLog->Logf( Type, TEXT("%s%s"), *s, *str );
}



/* ----------------------------------------------------------------------------
	FSwVersion
---------------------------------------------------------------------------- */
struct FSwVersion
{
	UINT raw;

	FSwVersion( UINT InRaw=-1 ) : raw(InRaw) {}
	UINT* GetRaw() { return &raw; }
	UINT GetDev() const { return raw & 0xFF; }
	UINT GetMinor() const { return (raw>>8) & 0xFF; }
	UINT GetMajor() const { return (raw>>16) & 0xFFFF; }
	operator UBOOL() const { return raw != -1; }
};


/* ----------------------------------------------------------------------------
	FSwSoundId
---------------------------------------------------------------------------- */
struct FSwSoundId
{
	DWORD Id;

	FSwSoundId::FSwSoundId() : Id(0) {}
	explicit FSwSoundId::FSwSoundId( DWORD InId ) : Id(InId) {}
	explicit FSwSoundId::FSwSoundId( void* InUserData ) : Id(reinterpret_cast<DWORD>(InUserData)) {}

	explicit FSwSoundId::FSwSoundId( AActor* InActor, ESoundSlot InSlot, UBOOL NoOverride )
	{
		SetActor(InActor);
		SetSlot(InSlot);
		SetNoOverride(NoOverride);
	}
	
	DWORD GetId() const							{ return Id; }
	DWORD GetIndex() const						{ return (Id >> 4) & 0x0FFFFFFF; }
	void* ToUserData() const					{ return reinterpret_cast<void*>(Id); }

	AActor* GetActor() const					{ return Cast<AActor>(UObject::GetIndexedObject(GetIndex())); }
	ESoundSlot GetSlot() const					{ return static_cast<ESoundSlot>((Id >> 1) & 0x07); }
	UBOOL GetNoOverride() const					{ return static_cast<UBOOL>(Id & 0x01); }

	void SetActor( AActor* Actor )				{ Id = (Id & 0x0F) | (Actor ? ((Actor->GetIndex() & 0x0FFFFFFF)<<4) : 0); }
	void SetSlot( ESoundSlot Slot )				{ Id = (Id & 0xFFFFFFF1) | ((Slot & 0x07) << 1); }
	void SetNoOverride( DWORD NoOverride )		{ Id = (Id & 0xFFFFFFFE) | (NoOverride & 0x01); }

	friend UBOOL operator ==( const FSwSoundId& A, const FSwSoundId& B )
	{
		return A.Id == B.Id;
	}
};


/* ----------------------------------------------------------------------------
	FSwChannelEnumerator
---------------------------------------------------------------------------- */
struct FSwChannelEnumerator
{
	FMOD::System*						System;
	TArray<FMOD::Channel*>				Channels;
	TArray<FMOD::Channel*>::TIterator	Iterator;

	explicit FSwChannelEnumerator( FMOD::System* InSystem, FMOD::ChannelGroup* InGroup=NULL )
		: System(InSystem), Iterator(Channels)
	{ 
		guard(FSwChannelEnumerator::FChannelEnumerator);

		// use master group if unspecified
		if( !InGroup )
			if( System->getMasterChannelGroup(&InGroup) != FMOD_OK )
				return;

		ParseGroup(InGroup);
		unguard;
	}

	void ParseGroup( FMOD::ChannelGroup* Group )
	{
		guard(FSwChannelEnumerator::ParseGroup);

		// Add our child channels
		int NumChannels = 0;
		FMOD::Channel* channel;
		if( Group->getNumChannels(&NumChannels) == FMOD_OK )
			for( int IdxChannel=0; IdxChannel<NumChannels; ++IdxChannel )
				if( Group->getChannel(IdxChannel,&channel) == FMOD_OK )
					Channels.AddItem(channel);

		// Add child channels from child groups
		int NumGroups = 0;
		FMOD::ChannelGroup* group;
		if( Group->getNumGroups(&NumGroups) == FMOD_OK )
			for( int IdxGroup=0; IdxGroup<NumGroups; ++IdxGroup )
				if( Group->getGroup(IdxGroup,&group) == FMOD_OK )
					ParseGroup(group);

		unguard;
	}

	void operator++()					{ ++Iterator; }
	operator UBOOL() const				{ return Iterator; }
	FMOD::Channel*& operator*() const	{ return *Iterator; }
};


/* ----------------------------------------------------------------------------
	FMOD::Channel accessors
---------------------------------------------------------------------------- */
inline UBOOL IsChannelValid( FMOD::Channel* Channel, UBOOL bCheckPlaying=0 )
{
	guard(IsChannelValid);
	FMOD_RESULT result;
	if( Channel )
	{
		bool test;
		result = Channel->isPlaying(&test);
		if( result == FMOD_OK && (!bCheckPlaying || test) )
		{
			return true;
		}
	}
	return 0;
	unguard;
}

inline FMOD::Sound* GetChannelSample( FMOD::Channel* Channel )
{
	guard(GetChannelSample);
	FMOD_RESULT result;
	if( Channel )
	{
		FMOD::Sound* sound;
		SWF_FMOD_CALL( Channel->getCurrentSound(&sound) );
		if( result == FMOD_OK )
		{
			return sound;
		}
	}
	return NULL;
	unguard;
}

inline UObject* GetChannelObject( FMOD::Channel* Channel )
{
	guard(GetChannelObject);
	FMOD_RESULT result;

	if( Channel )
	{
		FMOD::Sound* sound;
		SWF_FMOD_CALL( Channel->getCurrentSound(&sound) );
		if( result == FMOD_OK && sound )
		{
			void* data;
			SWF_FMOD_CALL( sound->getUserData(&data) );
			if( result == FMOD_OK && data )
			{
				UObject* object = static_cast<UObject*>(data);
				if( object->IsValid() )
					return object;
			}
		}
	}
	return NULL;
	unguard;
}

inline FSwSoundId GetChannelId( FMOD::Channel* Channel )
{
	guard(GetChannelId);
	FMOD_RESULT result;
	if( Channel )
	{
		void* data;
		SWF_FMOD_CALL( Channel->getUserData(&data) );
		if( result == FMOD_OK )
		{
			return FSwSoundId(data);
		}
	}
	return FSwSoundId();
	unguard;
}

inline AActor* GetChannelActor( FMOD::Channel* Channel )
{
	guard(GetChannelActor);
	return GetChannelId(Channel).GetActor();
	unguard;
}


/* ----------------------------------------------------------------------------
	Handle accessors for USound, UMusic and FMOD::Sound
---------------------------------------------------------------------------- */
inline FMOD::Sound* GetSoundSample( USound* Sound )
{
	return static_cast<FMOD::Sound*>(Sound->Handle);
}

inline FMOD::Sound* GetMusicSample( UMusic* Music )
{
	return static_cast<FMOD::Sound*>(Music->Handle);
}

inline UObject* GetSampleObject( FMOD::Sound* Sample )
{
	FMOD_RESULT result;

	if( Sample )
	{
		void* data = NULL;
		result = Sample->getUserData(&data);
		if( result == FMOD_OK && data )
		{
			UObject* object = static_cast<UObject*>(data);
			if( object->IsValid() )
				return object;
		}
	}
	return NULL;
}


/* ----------------------------------------------------------------------------
	Unit conversion
---------------------------------------------------------------------------- */
inline FLOAT ToFMODFloat( FLOAT f )
{
	return f*GUUToMeters;
}

inline FMOD_VECTOR ToFMODNormal( FVector v )
{
	// This means +X is right, +Y is up, and +Z is forwards.
	FMOD_VECTOR f = { v.Y, v.Z, v.X };
	return f;
}

inline FMOD_VECTOR ToFMODVector( FVector v )
{
	// This means +X is right, +Y is up, and +Z is forwards.
	FMOD_VECTOR f = { ToFMODFloat(v.Y), ToFMODFloat(v.Z), ToFMODFloat(v.X) };
	return f;
}


inline FLOAT ToUEFloat( FLOAT f )
{
	return f/GUUToMeters;
}

inline FVector ToUEVector( FMOD_VECTOR v )
{
	return FVector(ToUEFloat(v.z), ToUEFloat(v.x), ToUEFloat(v.y));
}

inline FVector ToUENormal( FMOD_VECTOR v )
{
	return FVector(v.z, v.x, v.y);
}



/* ----------------------------------------------------------------------------
	Music
---------------------------------------------------------------------------- */

/*inline FString GetSongSectionPostfix( EMusicMode Mode )
{
	switch( Mode )
	{
	case MUS_Ambient:		return TEXT("");
	case MUS_Combat:		return TEXT("_Combat");
	case MUS_Conversation:	return TEXT("_Dialog");
	case MUS_Outro:			return TEXT("_Outro");
	case MUS_Dying:			return TEXT("_Dying");
	default:				return TEXT("");
	}
}*/

inline FString GetSongSectionPostfix( BYTE SongSection )
{
	switch( SongSection )
	{
	case 0:		return TEXT("");
	case 1:		return TEXT("_Dying");
	case 2:		return TEXT("_Ambient");
	case 3:		return TEXT("_Combat");
	case 4:		return TEXT("_Dialog");
	case 5:		return TEXT("_Outro");
	default:	return TEXT("");
	}
}

struct FSwPosition
{
	UINT ms;
	UINT row;
	UINT pattern;
};


#endif
/* ----------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------- */