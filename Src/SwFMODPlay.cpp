/* ============================================================================
	SwFMODPlay.cpp:
	Copyright 2007 Roman Switch` Dzieciol. All Rights Reserved.
===============================================================================
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
============================================================================ */



// Includes.
#include "SwFMOD.h"



/*------------------------------------------------------------------------------------
	USwFMOD
------------------------------------------------------------------------------------*/

UBOOL USwFMOD::PlayMusic( UMusic* Music, FMOD::Channel* channel, BYTE SongSection, BYTE CdTrack, EMusicTransition Transition )
{
	guard(USwFMOD::PlayMusic);
	//SWF_LOG( NAME_DevSound, TEXT("%s >> %s :: [%s]"), SWF_PLOG, *ToStr(Music), *ToStr(SongSection), *ToStr(CdTrack), *ToStr(Transition)  );
	FMOD_RESULT result;

	if( !Music )
		return 0;

	/*if( channel )
	{
		// there can be only one music
		//SWF_LOG( NAME_DevSound, TEXT("%s :: %s :: StopMusic %s"), SWF_PLOG, *PrintChannel(musicchannel) );
		SWF_FMOD_RCALL0( musicchannel->setUserData(NULL) );
		SWF_FMOD_RCALL0( musicchannel->stop() );
	}*/


	// Non-modules use one UMusic object for each SongSection
	if( Music->FileType == SoundType_OGG
	||	Music->FileType == SoundType_MP2 
	||	Music->FileType == SoundType_MP3 )
	{
		Viewport->Actor->SongSection = 0;

		FString postfix = GetSongSectionPostfix(SongSection);
		if( postfix )
		{
			FString objname = FString::Printf( TEXT("%s%s.%s%s"), Music->GetName(), *postfix, Music->GetName(), *postfix);
			UMusic* newmusic = Cast<UMusic>(StaticLoadObject( Music->GetClass(), NULL, *objname, NULL, LOAD_NoWarn | LOAD_Quiet, NULL ));
			if( newmusic )
			{
				SWF_LOG( NAME_DevSound, TEXT("%s :: %s :: Dynamic Music: %s"), SWF_PLOG, *ToStr(objname) );
				Music = newmusic;
				Viewport->Actor->SongSection = SongSection;
			}
		}

		SongSection = Viewport->Actor->SongSection;
	}



	// Register sample
	FMOD::Sound* sample = RegisterMusicSample(Music);
	if( !sample )
		return 0;

	// Sample defaults
	FLOAT deffrequency;
	FLOAT defvolume;
	FLOAT defpanning;
	INT defpriority;
	FMOD_MODE fmode;
	SWF_FMOD_RCALL0( sample->getDefaults( &deffrequency, &defvolume, &defpanning, &defpriority ) );
	SWF_FMOD_RCALL0( sample->getMode(&fmode) );

	// Channel defaults
	FMOD::ChannelGroup* channelgroup = MusicChannels;
	FMOD_CHANNELINDEX channelindex = IsChannelValid(channel) ? FMOD_CHANNEL_REUSE : FMOD_CHANNEL_FREE;
	FLOAT frequency = deffrequency * 1.0f;
	FLOAT volume = defvolume * 1.0f;
	FLOAT panning = defpanning * 1.0f;
	INT priority = PriorityMusic;


	// Update FMOD to free finished channels
	guard(UpdateFMOD);
	SWF_FMOD_RCALL0( System->update() );
	unguard;

	// Play stream
	SWF_FMOD_RCALL0( System->playSound(channelindex, sample, true, &channel) );
//	SWF_FMOD_RCALL0( VerifyNewChannel(channel) );

	// Update channel
	SWF_FMOD_RCALL0( channel->setUserData(NULL) );
	SWF_FMOD_RCALL0( channel->setChannelGroup(channelgroup) );
	SWF_FMOD_RCALL0( channel->setMode(fmode) );
	if( !HasFlag(fmode,FMOD_3D) )
	{
		SWF_FMOD_RCALL0( channel->setPan(panning) );
	}
	SWF_FMOD_RCALL0( channel->setFrequency(frequency) );
	SWF_FMOD_RCALL0( channel->setVolume(volume) );
	SWF_FMOD_RCALL0( channel->setPriority(priority) );

	// restore position
	result = channel->setPosition(SongSection,FMOD_TIMEUNIT_MODORDER);
	FSwPosition pos = MusicPositions(SongSection);
	if( pos.ms != 0 )
	{
		channel->setPosition(pos.ms,FMOD_TIMEUNIT_MS);
	}
	else
	{
		// TODO:: doesn't work?!
		result = channel->setPosition(pos.pattern,FMOD_TIMEUNIT_MODPATTERN);
		result = channel->setPosition(pos.row,FMOD_TIMEUNIT_MODROW);
	}

	SWF_FMOD_RCALL0( channel->setPaused(false) );

	// Apply channel group's properties
	ApplyChannelGroup(channelgroup);

	// Update immediately
	Viewport->Actor->SongSection = SongSection;

	// Update FMOD to apply channel updates immediately
	guard(UpdateFMOD);
	SWF_FMOD_RCALL0( System->update() );
	unguard;

	//SWF_LOG( NAME_DevSound, TEXT("%s << %s :: [%s]"), SWF_PLOG, *ToStr(Music), *ToStr(SongSection), *ToStr(CdTrack), *ToStr(Transition)  );
	return 1;
	unguard;
}

UBOOL USwFMOD::PlaySound
(
	AActor*	Actor,
	INT		Id,
	USound*	Sound,
	FVector	Location,
	FLOAT	Volume,
	FLOAT	Radius,
	FLOAT	Pitch
)
{
	guard(USwFMOD::PlaySound);
	//SWF_LOG( NAME_DevSound, TEXT("%s >> %s :: [%s],[%s],[%s],[%s],[%s],[%s],[%s]"), SWF_PLOG, *ToStr(FSwSoundId(Id)), *ToStr(Actor,true), *ToStr(Sound), *ToStr(Location), *ToStr(Volume), *ToStr(Radius), *ToStr(Pitch) );
	FMOD_RESULT result;

	if( !Viewport || !Sound )
		return 0;

	// Sound slot rules
	FSwSoundId SoundId = FSwSoundId(Id);
	if( SoundId.GetSlot() != SLOT_None )
	{
		for( FSwChannelEnumerator it(System); it; ++it )
		{
			FMOD::Channel* channel = *it;
			if( IsChannelValid(channel) )
			{
				FSwSoundId cid = GetChannelId(channel);

				// If Actor is the same and Slot is the same:
				// - prevent this sound from plaing if NoOverride
				// - stop the old sound otherwise
				if( cid.GetSlot() == SoundId.GetSlot()
				&&	cid.GetActor() == SoundId.GetActor() )
				{
					if( cid.GetNoOverride() )
					{
						//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: NO OVERRIDE <%s> <%s>"), SWF_PLOG, *ToStr(FSwSoundId(Id)), *PrintChannel(channel) );
						return 0;
					}
					//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: STOP <%s> <%s>"), SWF_PLOG, *ToStr(SoundId), *PrintChannel(channel) );
					SWF_FMOD_CALL( channel->setUserData(NULL) );
					SWF_FMOD_CALL( channel->stop() );
				}
			}
		}
	}

	// Register sample
	FMOD::Sound* sample = RegisterSoundSample(Sound);
	if( !sample )
		return 0;

	// Sample defaults
	FLOAT deffrequency;
	FLOAT defvolume;
	FLOAT defpanning;
	INT defpriority;
	FMOD_MODE fmode;
	SWF_FMOD_RCALL0( sample->getDefaults( &deffrequency, &defvolume, &defpanning, &defpriority ) );
	SWF_FMOD_RCALL0( sample->getMode(&fmode) );

	// Channel defaults
	FMOD::ChannelGroup* channelgroup = EffectChannels;
	FMOD_CHANNELINDEX channelindex = FMOD_CHANNEL_FREE;
	FLOAT mindist = ToFMODFloat(DistanceMin);
	FLOAT radius = ToFMODFloat(Radius);
	TSwSortMinMax( mindist, radius );
	FMOD_VECTOR location = ToFMODVector(Location); 
	FMOD_VECTOR velocity = ToFMODNormal(FVector(0,0,0));
	FLOAT frequency = deffrequency * Pitch;
	FLOAT volume = defvolume * Volume;
	FLOAT panning = defpanning * 1.0f;
	INT priority = PrioritySound; 

	// 2D or 3D?
	AActor* ViewActor = Viewport->Actor->ViewTarget ? Viewport->Actor->ViewTarget : Viewport->Actor;
	UBOOL bIs3D = Actor && (b3DCameraSounds || Actor != ViewActor);
	if( bIs3D )
	{
		fmode &= ~FMOD_2D;
		fmode |= FMOD_3D | FMOD_3D_LINEARROLLOFF;

		// Radius, location & Velocity
		FMOD_VECTOR location = ToFMODVector(Actor->Location); 
		FMOD_VECTOR velocity = ToFMODVector(Actor->Velocity); 

		SWF_VERIFY_FLOAT( mindist );
		SWF_VERIFY_FLOAT( radius );
		SWF_VERIFY_FMODVECTOR( location );
		SWF_VERIFY_FMODVECTOR( velocity );
	}
	else
	{
		fmode &= ~FMOD_3D;
		fmode |= FMOD_2D;
	}

	// Sound effects other than WAV go into compressed group (conversations most likely)
	if( Sound->FileType != SoundType_WAV )
	{
		priority = PrioritySpeech;
		channelgroup = CompressedChannels;
		//fmode |= FMOD_CREATESTREAM;
	}
	
	// Ambient sounds
	if( SoundId.GetSlot() == SLOT_Ambient )
	{
		priority = PriorityAmbient;
		channelgroup = AmbientChannels;
		fmode &= ~FMOD_LOOP_OFF;
		fmode |= FMOD_LOOP_NORMAL;
	}


	// Update FMOD to free finished channels
	guard(UpdateFMOD);
	SWF_FMOD_RCALL0( System->update() );
	unguard;

	// Play sound
	FMOD::Channel* channel;
    SWF_FMOD_RCALL0( System->playSound(channelindex, sample, true, &channel) );
//	SWF_FMOD_RCALL0( VerifyNewChannel(channel) );

	// Update channel
	SWF_FMOD_RCALL0( channel->setUserData( SoundId.ToUserData() ) );
	SWF_FMOD_RCALL0( channel->setChannelGroup(channelgroup) );
	SWF_FMOD_RCALL0( channel->setMode(fmode) );
	if( HasFlag(fmode,FMOD_3D) )
	{
		SWF_FMOD_RCALL0( channel->set3DMinMaxDistance(mindist, radius) );
		SWF_FMOD_RCALL0( channel->set3DAttributes(&location,&velocity) );
	}
	else
	{
		SWF_FMOD_RCALL0( channel->setPan(panning) );
	}
	SWF_FMOD_RCALL0( channel->setFrequency(frequency) );
	SWF_FMOD_RCALL0( channel->setVolume(volume) );
	SWF_FMOD_RCALL0( channel->setPriority(priority) );
	SWF_FMOD_RCALL0( channel->setPaused(false) );

	// Apply channel group's properties
	ApplyChannelGroup(channelgroup);

	// Update FMOD to apply channel updates immediately
	guard(UpdateFMOD);
	SWF_FMOD_RCALL0( System->update() );
	unguard;

	//SWF_LOG( NAME_DevSound, TEXT("%s << %s :: [%s],[%s],[%s],[%s],[%s],[%s],[%s]"), SWF_PLOG, *ToStr(Actor,true), *ToStr(Id), *ToStr(Sound), *ToStr(Location), *ToStr(Volume), *ToStr(Radius), *ToStr(Pitch) );
	return 1;
	unguard;
}

void USwFMOD::ApplyChannelGroup( FMOD::ChannelGroup* ChannelGroup )
{
	guard(USwFMOD::ApplyChannelGroup);
	FMOD_RESULT result;

	FLOAT cvol;
	SWF_FMOD_CALL( ChannelGroup->getVolume(&cvol) );
	SWF_FMOD_CALL( ChannelGroup->setVolume(cvol) );

	unguard;
}

void USwFMOD::NoteDestroy( AActor* Actor )
{
	guard(USwFMOD::NoteDestroy);
	//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: [%s]"), SWF_PLOG, *ToStr(Actor) );
	FMOD_RESULT result;
	INT count = 0;

	if( Actor && Actor->IsValid() )
	{
		// Stop sound
		for( FSwChannelEnumerator it(System); it; ++it )
		{
			FMOD::Channel* channel = *it;
			if( IsChannelValid(channel) )
			{
				FSwSoundId id = GetChannelId(channel);
				if( id.GetActor() == Actor )
				{
					//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: %s"), SWF_PLOG, *PrintChannel(channel) );
					if( id.GetSlot() == SLOT_Ambient )
					{
						SWF_FMOD_CALL( channel->setUserData(NULL) );
						SWF_FMOD_CALL( channel->stop() );
					}
					else
					{
						id.SetActor(NULL);
						SWF_FMOD_CALL( channel->setUserData(id.ToUserData()) );
					}
					++count;
				}
			}
		}

		// Update system
		if( count )
		{
			guard(UpdateFMOD);
			SWF_FMOD_CALL( System->update() );
			unguard;
		}
	}

	unguard;
}

void USwFMOD::StopSoundId( INT Id )
{
	guard(USwFMOD::StopSoundId);
	//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: [%s]"), SWF_PLOG, *ToStr(FSwSoundId(Id)) );
	FMOD_RESULT result;
	INT count = 0;
	
	if( Id != 0 )
	{
		// Stop sound
		FSwSoundId soundid = FSwSoundId(Id);
		for( FSwChannelEnumerator it(System); it; ++it )
		{
			FMOD::Channel* channel = *it;
			if( IsChannelValid(channel) )
			{
				FSwSoundId cid = GetChannelId(channel);
				if( cid == soundid )
				{
					//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: [%s] [%s]"), SWF_PLOG, *ToStr(soundid), *PrintChannel(channel) );
					SWF_FMOD_CALL( channel->setUserData(NULL) );
					SWF_FMOD_CALL( channel->stop() );
					++count;
				}
			}
		}

		// Update system
		if( count )
		{
			guard(UpdateFMOD);
			SWF_FMOD_CALL( System->update() );
			unguard;
		}
	}

	unguard;
}




/* ----------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------- */
