/* ============================================================================
	SwFMODUpdate.cpp:
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

void USwFMOD::Update( FPointRegion Region, FCoords& Coords )
{
	guard(USwFMOD::Update);
	FMOD_RESULT result;
	//SWF_LOG( NAME_DevSound, TEXT("%s >> %s :: [%s],[%s]"), SWF_PLOG, *ToStr(Region), *ToStr(Coords) );

	if( !Viewport || !Viewport->IsValid() || !Viewport->Actor || !Viewport->Actor->IsValid() )
		return;

	// Load geometry
	if( bOcclusion )
	{
		static ULevel* level = NULL;
		if( Viewport->Actor->GetLevel() != level )
		{
			level = Viewport->Actor->GetLevel();
			LoadGeometry();
		}
	}

	// Time passes...
	DOUBLE DeltaTime = appSeconds() - LastTime;
	LastTime += DeltaTime;
	DeltaTime = Clamp( DeltaTime, 0.0, 1.0 );

	// Get viewactor
	AActor* ViewActor = Viewport->Actor->ViewTarget ? Viewport->Actor->ViewTarget : Viewport->Actor;
	
	// Is viewport realtime ?
	UBOOL Realtime = Viewport->IsRealtime() && Viewport->Actor->Level->Pauser == TEXT("");
	
	// Default viewport coords
	FVector location = FVector(0,0,0);
	FVector velocity = FVector(0,0,0);
	FVector forward = FVector(1,0,0);
	FVector up = FVector(0,0,1);


	//
	// Update listener
	//
	guard(USwFMODAudio::Update::UpdateListener);

	// Use ViewActor coords
	FCoords coords = GMath.UnitCoords / ViewActor->Rotation;;
	coords.Origin = ViewActor->Location;

	// Set viewport coords
	location = coords.Origin;
	velocity = ViewActor->Velocity;
	forward = coords.XAxis;
	up = coords.ZAxis;

	// verify
	SWF_VERIFY_FVECTOR(location);
	SWF_VERIFY_FVECTOR(velocity);
	SWF_VERIFY_FVECTOR_NORMAL(forward);
	SWF_VERIFY_FVECTOR_NORMAL(up);

	// Default listener coords
	FMOD_VECTOR fm_pos = {0,0,0};
	FMOD_VECTOR fm_vel = {0,0,0};
	FMOD_VECTOR fm_fwd = {0,0,1};
	FMOD_VECTOR fm_up  = {0,1,0};

	// Set listener coords
	fm_pos = ToFMODVector(location);
	fm_vel = ToFMODVector(velocity);
	fm_fwd = ToFMODNormal(forward);
	fm_up = ToFMODNormal(up);

	// verify
	SWF_VERIFY_FMODVECTOR(fm_pos);
	SWF_VERIFY_FMODVECTOR(fm_vel);
	SWF_VERIFY_FMODVECTOR_NORMAL(fm_fwd);
	SWF_VERIFY_FMODVECTOR_NORMAL(fm_up);

	// Update
	SWF_FMOD_CALL( System->set3DListenerAttributes( 0, &fm_pos, &fm_vel, &fm_fwd, &fm_up ) );
	unguard;


	//
	// Zone effects
	//
	guard(USwFMODAudio::Update::UpdateZone);

	/*// Default zone properties
	UBOOL bWaterZone = 0;
	UBOOL bReverbZone = 0;
	UBOOL bRaytraceReverb = 0;
	BYTE MasterGain = 100;
	INT CutoffHz = 6000;
	BYTE Delay[6] = {20,34,0,0,0,0};
	BYTE Gain[6] = {150,70,0,0,0,0};

	// Get zone properties
	if( Region.Zone && Region.Zone->IsValid() )
	{
		bWaterZone = Region.Zone->bWaterZone;
		bReverbZone = Region.Zone->bReverbZone;
		bRaytraceReverb = Region.Zone->bRaytraceReverb;
		MasterGain = Region.Zone->MasterGain;
		CutoffHz = Region.Zone->CutoffHz;
		appMemcpy(Delay, Region.Zone->Delay, 6*sizeof(BYTE));
		appMemcpy(Gain, Region.Zone->Gain, 6*sizeof(BYTE));
	}*/
	unguard;



	//
	// Update sounds.
	//
	guard(USwFMODAudio::Update::UpdateSounds);

	// Iterate through all channels
	for( FSwChannelEnumerator it(System); it; ++it )
	{
		FMOD::Channel* channel = *it;
		if( !IsChannelValid(channel) )
			continue;

		// Channel data
		FMOD::Sound* sample = GetChannelSample(channel);
		if( !sample )
			continue;

		UObject* object = GetSampleObject(sample);
		if( !object )
			continue;
		USound* sound = Cast<USound>(object);

		FSwSoundId id = GetChannelId(channel);
		AActor* actor = id.GetActor();

		FMOD_MODE fmode;
		SWF_FMOD_CALL( channel->getMode(&fmode) );
		UBOOL bIs3D = HasFlag(fmode,FMOD_3D);

		// Sample defaults
		FLOAT deffrequency;
		FLOAT defvolume;
		FLOAT defpanning;
		INT defpriority;
		SWF_FMOD_CALL( sample->getDefaults( &deffrequency, &defvolume, &defpanning, &defpriority ) );
	

		if( sound )
		{
			if( actor && actor->IsValid() )
			{
				// Ambient sounds
				if( id.GetSlot() == SLOT_Ambient )
				{
					if(	actor->AmbientSound != sound
					||	FDist(location, actor->Location) > actor->WorldSoundRadius() + AmbientHysteresis
					||  !Realtime )
					{
						// Stop ambient sound
						//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: Ambient OUT [%d]"), SWF_PLOG, *ToStr(id) );
						SWF_FMOD_CALL( channel->setUserData(NULL) );
						SWF_FMOD_CALL( channel->stop() );
						continue;
					}
					else
					{
						// Update ambient sound properties.
						FLOAT volume = actor->SoundVolume / 255.0f;
						FLOAT frequency = (actor->SoundPitch / 64.0f) * deffrequency;

						SWF_VERIFY_FLOAT(volume);
						SWF_VERIFY_FLOAT(frequency);

						SWF_FMOD_CALL( channel->setVolume( volume ) );
						SWF_FMOD_CALL( channel->setFrequency( frequency ) );

						if( bIs3D )
						{
							// Update 3D sound properties
							FLOAT mindist = ToFMODFloat(DistanceMin);
							FLOAT radius = ToFMODFloat( actor->WorldSoundRadius() );
							TSwSortMinMax( mindist, radius );

							SWF_VERIFY_FLOAT(radius);
							SWF_VERIFY_FLOAT(mindist);

							SWF_FMOD_CALL( channel->set3DMinMaxDistance( mindist, radius ) );
						}
					}
				}

				if( bIs3D )
				{
					// Update 3D sound properties
					FMOD_VECTOR snd_pos = ToFMODVector(actor->Location);
					FMOD_VECTOR snd_vel = ToFMODVector(actor->Velocity);
			
					SWF_VERIFY_FMODVECTOR(snd_pos);							
					SWF_VERIFY_FMODVECTOR(snd_vel);

					SWF_FMOD_CALL( channel->set3DAttributes(&snd_pos, &snd_vel) );
				}
			}
		}
	}
	unguard;


	//
	// Play ambient sounds
	//
	if( Realtime )
	{
		guard(USwFMODAudio::Update::PlayAmbient);
		for( INT i=0; i<Viewport->Actor->GetLevel()->Actors.Num(); ++i )
		{
			AActor* Actor = Viewport->Actor->GetLevel()->Actors(i);
			if(	Actor
			&&	Actor->AmbientSound
			&&	FDist(location, Actor->Location) <= Actor->WorldSoundRadius() )
			{
				FSwSoundId ambientid = FSwSoundId(Actor,SLOT_Ambient,0);
				//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: Ambient TEST IN [%s]"), SWF_PLOG, *ToStr(ambientid) );

				// Find this sound in currently playing ones
				FMOD::Channel* ambientchannel = NULL;
				for( FSwChannelEnumerator it(System,AmbientChannels); it; ++it )
				{
					FMOD::Channel* channel = *it;
					if( IsChannelValid(channel) && GetChannelId(channel) == ambientid )
					{
						//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: Ambient FOUND IN [%s]"), SWF_PLOG, *ToStr(GetChannelId(channel)) );
						ambientchannel = channel;
						break;
					}
				}

				// If not found play ambient
				if( !ambientchannel )
				{
					//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: Ambient PLAY IN [%s][%s]"), SWF_PLOG, *ToStr(ambientid), *ToStr(Actor->AmbientSound) );
					PlaySound( Actor, ambientid.GetId(), Actor->AmbientSound, Actor->Location, Actor->SoundVolume/255.0f, Actor->WorldSoundRadius(), Actor->SoundPitch/64.0f );
				}
			}
		}
		unguard;
	}
	

	//
	// Music
	//
	guard(UpdateMusic)

/*	REQUIREMENTS

	SongSection is updated at realtime by audio sys
	MTRAN_Fade* only fade out, not in
	music changes caused by transition only
	ttransition reset on change
	MTRAN_None = don't change
	MTRAN_Instant = instant change
	MTRAN_Segue = seamless?
	MTRAN_Fade = 1s fade
	MTRAN_FastFade = 1/3s fade
	MTRAN_SlowFade = 5s fade
*/
	// find music channel
	FMOD::Channel* musicchannel = NULL;
	for( FSwChannelEnumerator it(System,MusicChannels); it; ++it )
	{
		FMOD::Channel* channel = *it;
		if( !IsChannelValid(channel) )
			continue;

		if( !musicchannel )
		{
			musicchannel = channel;
		}
		else
		{
			// there can be only one music
			SWF_LOG( NAME_DevSound, TEXT("%s :: %s :: StopMusic %s"), SWF_PLOG, *PrintChannel(channel) );
			SWF_FMOD_CALL( channel->setUserData(NULL) );
			SWF_FMOD_CALL( channel->stop() );
		}
	}

	if( Viewport->Actor->Transition != MTRAN_None )
	{
		// init fading
		if( MusicFade < 0 )
		{
			SWF_LOG( NAME_DevSound, TEXT("%s :: %s :: Music transition %s S:%s T:%s "), SWF_PLOG
				, *ToStr(Viewport->Actor->Song)
				, *ToStr(Viewport->Actor->SongSection)
				, *ToStr(Viewport->Actor->Transition));

			switch( Viewport->Actor->Transition )
			{
				case MTRAN_Instant:		MusicFadeTime = -1.0f;	break;	// Instant
				case MTRAN_Segue:		MusicFadeTime = -1.0f;			// Instant precached
					if( Viewport->Actor->Song && !Viewport->Actor->Song->Handle )
						RegisterMusic(Viewport->Actor->Song);
					break;
				case MTRAN_Fade:		MusicFadeTime = 1.0f;	break;	// 1s fadeout
				case MTRAN_FastFade:	MusicFadeTime = 0.33f;	break;	// 1/3s fadeout
				case MTRAN_SlowFade:	MusicFadeTime = 5.0f;	break;	// 5s fadeout
				default:				MusicFadeTime = -1.0f;	break;	// Unknown,instant
					
			}
			MusicFade = MusicFadeTime;
		}

		// deduct delta
		MusicFade -= DeltaTime;
		//SWF_LOG( NAME_DevSound, TEXT("%s << %s :: MusicFade %s %s"), SWF_PLOG, *ToStr(MusicFade), *ToStr(MusicFadeTime) );

		if( MusicFade > 0 )
		{
			// fade volume
			if( musicchannel && MusicFadeTime > 0 )
			{
				SWF_FMOD_CALL( musicchannel->setVolume( MusicFade / MusicFadeTime) );
			}
		}
		else
		{
			// play new
			MusicFade = -1;
			Viewport->Actor->Transition = MTRAN_None;
			PlayMusic( Viewport->Actor->Song, musicchannel, Viewport->Actor->SongSection, Viewport->Actor->CdTrack, static_cast<EMusicTransition>(Viewport->Actor->Transition) );
		}
	}
	else
	{
		// Update section
		if( musicchannel )
		{
			// update section
			// FIXME:: getPosition doesn't work with volume 0 (virtual?)
			UINT sec = 0;
			result = musicchannel->getPosition(&sec,FMOD_TIMEUNIT_MODORDER);
			if( result == FMOD_OK )
			{
				Viewport->Actor->SongSection = sec;
			}

			// Update position
			if( IsChannelValid(musicchannel) )
			{
				UINT row = 0;
				result = musicchannel->getPosition(&row,FMOD_TIMEUNIT_MODROW);
				if( result == FMOD_OK )
				{
					// IT/MOD/XM
					UINT pattern = 0;
					result = musicchannel->getPosition(&pattern,FMOD_TIMEUNIT_MODPATTERN);
					if( result == FMOD_OK )
					{
						MusicPositions(Viewport->Actor->SongSection).row = row;
						MusicPositions(Viewport->Actor->SongSection).pattern = pattern;
					}
				}
				else
				{
					// MPEG/OGG
					UINT ms = 0;
					result = musicchannel->getPosition(&ms,FMOD_TIMEUNIT_MS);
					if( result == FMOD_OK )
					{
						MusicPositions(Viewport->Actor->SongSection).ms = ms;
					}
				}
			}
		}
		else if( Viewport->Actor->Song && VolumeMusic > 0 )
		{
			// Restart missing/dropped song (bad channel priorities?)
			SWF_LOG( NAME_DevSound, TEXT("%s :: %s :: Restarting missing song %s S:%s T:%s "), SWF_PLOG
				, *ToStr(Viewport->Actor->Song)
				, *ToStr(Viewport->Actor->SongSection)
				, *ToStr(Viewport->Actor->Transition));
			Viewport->Actor->Transition = MTRAN_Instant;
		}
	}



	unguard;

	// Update FMOD
	guard(UpdateFMOD);
	SWF_FMOD_CALL( System->update() );
	unguard;

	//SWF_LOG( NAME_DevSound, TEXT("%s << %s :: [%s],[%s]"), SWF_PLOG, *ToStr(Region), *ToStr(Coords) );
	unguard;
}



/* ----------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------- */
