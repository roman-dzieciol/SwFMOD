/* ============================================================================
	SwFMODRegister.cpp:
	Copyright 2007 Roman Switch` Dzieciol. All Rights Reserved.
===============================================================================
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
============================================================================ */

// TODO:: Duration

// Includes.
#include "SwFMOD.h"



void USwFMOD::RegisterSound( USound* Sound )
{
	guard(USwFMOD::RegisterSound);
	//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: [%s]"), SWF_LOGP, *ToStr(Sound) );
	FMOD_RESULT result;

	checkSlow(Sound);
	if( !Sound->Handle )
	{
		FMOD::Sound* sound;

		// Load the data.
		Sound->Data.LoadSpecial();
		SWF_LOG( NAME_DevSound, TEXT("Register sound: %s (%i)"), Sound->GetPathName(), Sound->Data.Num() );
		check(Sound->Data.Num()>0);

		// Sound extended data
#pragma pack(push)
#pragma pack(8)
		FMOD_CREATESOUNDEXINFO exinfo;
		appMemset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
		exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		exinfo.length = Sound->Data.Num();
#pragma pack(pop)

		// Sound flags
		FMOD_MODE fmode = 0
			| FMOD_SOFTWARE
			| FMOD_IGNORETAGS
		//	| FMOD_LOOP_OFF
		//	| FMOD_2D
			| FMOD_OPENMEMORY
			| FMOD_CREATESAMPLE
			| FMOD_3D 
			| FMOD_3D_LINEARROLLOFF
			;
	

		// Format-specific flags
		if( Sound->FileType == SoundType_WAV )
		{
			// WAV
			exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_WAV;
		}
		else if( Sound->FileType == SoundType_MP2 || Sound->FileType == SoundType_MP3 )
		{
			// MP3 
			fmode |= FMOD_ACCURATETIME;
			exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_MPEG;
		}
		else
		{
			// Unknown, try anyway
			fmode |= FMOD_ACCURATETIME;
		}

		// Create sound
		SWF_FMOD_CALL( System->createSound( static_cast<const char*>(Sound->Data.GetData()), fmode, &exinfo, &sound ));
		if( result == FMOD_OK )
		{
			// Register the sound.
			Sound->Handle = sound;
			SWF_FMOD_CALL( sound->setUserData(Sound) );

			unsigned int length;
			SWF_FMOD_CALL( sound->getLength(&length,FMOD_TIMEUNIT_MS) );
			Sound->Duration = length*0.001f;
		}
		else
		{
			SWF_LOG_WARN( FString::Printf(TEXT("Couldn't register sound [%s][%i][%s]"), Sound->GetPathName(), Sound->Data.Num(), *ToStr(result)) );
		}
		
		// Unload the data.
		Sound->Data.Unload();
	}

	unguard;
}

void USwFMOD::UnregisterSound( USound* Sound )
{
	guard(USwFMOD::UnregisterSound);
	//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: [%s]"), SWF_LOGP, *ToStr(Sound) );
	FMOD_RESULT result;

	check(Sound);
	if( Sound->Handle )
	{
		SWF_LOG( NAME_DevSound, TEXT("Unregister sound: %s"), Sound->GetFullName() );

		// Stop this sound.
		FMOD::Sound* sound = GetSoundSample(Sound);
		if( sound )
		{
			SWF_FMOD_CALL( sound->setUserData(NULL) );
			SWF_FMOD_CALL( sound->release() );
		}

		// Reset handle
		Sound->Handle = NULL;
	}

	unguard;
}

void USwFMOD::RegisterMusic( UMusic* Music )
{
	guard(USwFMOD::RegisterMusic);
	//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: [%s]"), SWF_LOGP, *ToStr(Music) );
	FMOD_RESULT result;

	checkSlow(Music);
	if( !Music->Handle )
	{
		FMOD::Sound* sound;

		// Load the data.
		Music->Data.Load();
		SWF_LOG( NAME_DevSound, TEXT("Register music: %s (%i)"), Music->GetPathName(), Music->Data.Num() );
		check(Music->Data.Num()>0);

		// Sound extended data
#pragma pack(push)
#pragma pack(8)
		FMOD_CREATESOUNDEXINFO exinfo;
		appMemset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
		exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		exinfo.length = Music->Data.Num();
#pragma pack(pop)

		// Sound flags
		FMOD_MODE fmode = 0
			| FMOD_SOFTWARE
		//	| FMOD_IGNORETAGS
		//	| FMOD_LOOP_NORMAL
			| FMOD_2D
			| FMOD_OPENMEMORY
		//	| FMOD_ACCURATETIME
			| FMOD_CREATESTREAM
			;
	
		// Format-specific flags
		if( Music->FileType == SoundType_OGG )
		{
			exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_OGGVORBIS;
			fmode |= FMOD_LOOP_NORMAL;
		}
		else if( Music->FileType == SoundType_MP2 || Music->FileType == SoundType_MP3 )
		{
			exinfo.suggestedsoundtype = FMOD_SOUND_TYPE_MPEG;
			fmode |= FMOD_LOOP_NORMAL;
		}
		else
		{
		}

		// Create sound
		SWF_FMOD_CALL( System->createStream( static_cast<const char*>(Music->Data.GetData()), fmode, &exinfo, &sound ));
		if( result == FMOD_OK )
		{
			// Register the sound.
			Music->Handle = sound;
			SWF_FMOD_CALL( sound->setUserData(Music) );
		}
		else
		{
			SWF_LOG_WARN( FString::Printf(TEXT("Couldn't register music [%s][%i][%s]"), Music->GetPathName(), Music->Data.Num(), *ToStr(result)) );
		}
		
		// Do not unload yet.
		//Music->Data.Unload();
	}

	unguard;
}

void USwFMOD::UnregisterMusic( UMusic* Music )
{
	guard(USwFMOD::UnregisterMusic);
	//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: [%s]"), SWF_LOGP, *ToStr(Music) );
	FMOD_RESULT result;

	check(Music);
	if( Music->Handle )
	{
		SWF_LOG( NAME_DevSound, TEXT("Unregister music: %s"), Music->GetFullName() );

		// Stop this sound.
		FMOD::Sound* sound = GetMusicSample(Music);
		if( sound )
		{
			SWF_FMOD_CALL( sound->setUserData(NULL) );
			SWF_FMOD_CALL( sound->release() );
		}

		// Reset handle
		Music->Handle = NULL;

		// Unload
		Music->Data.Unload();
	}

	unguard;
}


FMOD::Sound* USwFMOD::RegisterSoundSample( USound* Sound )
{
	if( Sound )
	{
		if( !Sound->Handle )
			RegisterSound(Sound);
		return GetSoundSample(Sound);
	}
	return NULL;
}

FMOD::Sound* USwFMOD::RegisterMusicSample( UMusic* Music )
{
	if( Music )
	{
		if( !Music->Handle )
			RegisterMusic(Music);
		return GetMusicSample(Music);
	}
	return NULL;
}


/* ----------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------- */
