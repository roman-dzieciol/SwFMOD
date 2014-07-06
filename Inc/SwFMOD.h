/* ============================================================================
	SwFMOD.h:
	Copyright 2007 Roman Switch` Dzieciol. All Rights Reserved.
===============================================================================
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
===============================================================================
 - Keep in mind alignment: UE uses 4 bytes, FMOD 8 bytes.
 - NAME_DevSound log is suppressed by defualt
============================================================================ */
#ifndef SWFMOD_H
#define SWFMOD_H


// FMOD Includes.
#pragma pack(push)
#pragma pack(8)
#include "fmod.hpp"
#include "fmod_errors.h"
#pragma pack(pop)


// Unreal Includes.
#include "Core.h"
#include "Engine.h"
#include "UnRender.h"


// Setup
#pragma DISABLE_OPTIMIZATION
#define SWFMOD_API DLL_EXPORT


// Libraries.
#pragma comment(lib, "Core")
#pragma comment(lib, "Engine")
#pragma comment(lib, "fmodex_vc")


// Globals
extern SWFMOD_API FName SWFMOD_SwFMOD;
extern SWFMOD_API FName SWFMOD_SwFMOD_WARNING;
extern SWFMOD_API FLOAT GUUToMeters;
extern SWFMOD_API DWORD GSwfErrorCount;

//extern char *FMOD_ErrorString(FMOD_RESULT errcode);

#define SWF_NAME			(EName)SWFMOD_SwFMOD.GetIndex()
#define SWF_NAME_WARNING	(EName)SWFMOD_SwFMOD_WARNING.GetIndex()


// Forward declarations
struct FSwSoundId;
struct FSwVersion;


// Utils
#include "SwFMODMacro.h"
#include "SwFMODToStr.h"
#include "SwFMODUtil.h"



/* ----------------------------------------------------------------------------
	USwFMOD.
-------------------------------------------------------------------------------
	Error checking rules:
	Construction: no-fail
	Destruction: no-fail
	Initialization: on failure log and return 0 to self destruct
	Other: on failure log, ignore or self destruct, never terminate the app
---------------------------------------------------------------------------- */

class SWFMOD_API USwFMOD : public UAudioSubsystem
{
	DECLARE_CLASS(USwFMOD,UAudioSubsystem,CLASS_Config)

	// Configuration.
	FLOAT			VolumeMusic;
	FLOAT			VolumeSound;
	FLOAT			VolumeSpeech;
	FLOAT			AmbientFactor;
	FLOAT			AmbientHysteresis;

	BYTE			SampleRate;
	BYTE			SampleFormat;
	BYTE			SampleInterpolation;

	INT				b3DCameraSounds;
	INT				bLowSoundQuality;

	DWORD			Channels;
	DWORD			VirtualChannels;
	FLOAT			VirtualThreshold;

	BYTE			PriorityMusic;
	BYTE			PrioritySpeech;
	BYTE			PriorityAmbient;
	BYTE			PrioritySound;

	INT				StatGlobal;
	INT				StatChannelGroup;
	INT				StatChannels;
	INT				StatRender;
	INT				StatPositions;
	INT				bLogPlugins;
	INT				bPrecache;

	FLOAT			ToMeters;
	FLOAT			DopplerScale;
	FLOAT			DistanceFactor;
	FLOAT			DistanceMin;
	FLOAT			RolloffScale;

	INT				bHRTF;
	FLOAT			HRTFMinAngle;
	FLOAT			HRTFMaxAngle;
	FLOAT			HRTFFreq;

	INT				bOcclusion;

	INT				OverrideOutput;
	INT				OverrideOutputChannels;
	INT				OverrideInputChannels;
	INT				OverrideDSPBufferLength;
	INT				OverrideDSPBufferCount;
	INT				OverrideInitFlags;
	INT				OverrideDebugFlags;
	INT				OverrideSpeakerMode;

	INT				Driver;
	INT				Min2D;
	INT				Max2D;
	INT				Min3D;
	INT				Max3D;


	// Internals.
	UBOOL			bInitialized;
	UViewport*		Viewport;

	FLOAT				MusicFade;
	FLOAT				MusicFadeTime;
	DOUBLE				LastTime;
	TArray<FSwPosition>	MusicPositions;


	// FMOD
	FMOD::System*		System;
	FMOD::ChannelGroup* AmbientChannels;
	FMOD::ChannelGroup* EffectChannels;
	FMOD::ChannelGroup* MusicChannels;
	FMOD::ChannelGroup* CompressedChannels;
	FMOD::ChannelGroup* MasterChannel;
	FMOD::SoundGroup*	MusicGroup;
	FMOD::SoundGroup*	MasterGroup;
	FMOD::Geometry*		Geometry;
	

	// Static
	static const FName	SoundType_WAV;
	static const FName	SoundType_MP2;
	static const FName	SoundType_MP3;
	static const FName	SoundType_OGG;
	static const DWORD	OutputRates[];


	// Constructor.
	USwFMOD();
	void StaticConstructor();


	// UObject interface.
	virtual void Destroy();
	virtual void PostEditChange();
	virtual void ShutdownAfterError();


	// UAudioSubsystem interface.
	virtual UBOOL Init();
	virtual void SetViewport( UViewport* Viewport );
	virtual UBOOL Exec( const TCHAR* Cmd, FOutputDevice& Ar );
	virtual void Update( FPointRegion Region, FCoords& Listener );
	virtual void RegisterMusic( UMusic* Music );
	virtual void RegisterSound( USound* Music );
	virtual void UnregisterSound( USound* Sound );
	virtual void UnregisterMusic( UMusic* Music );
	virtual UBOOL PlaySound( AActor* Actor, INT Id, USound* Sound, FVector Location, FLOAT Volume, FLOAT Radius, FLOAT Pitch );
	virtual void StopSoundId( INT Id );		// DEUS_EX CNN
	virtual void NoteDestroy( AActor* Actor );
	virtual UBOOL GetLowQualitySetting();
	virtual UViewport* GetViewport();
	virtual void RenderAudioGeometry( FSceneNode* Frame );
	virtual void PostRender( FSceneNode* Frame );
	virtual void SetInstantSoundVolume( BYTE NewVolume );		// DEUS_EX CNN - instantly sets the sound volume
	virtual void SetInstantSpeechVolume( BYTE NewVolume );		// DEUS_EX CNN - instantly sets the speech volume
	virtual void SetInstantMusicVolume( BYTE NewVolume );		// DEUS_EX CNN - instantly sets the music volume


	// USwFMOD interface
	void VerifyConfig();
	virtual UBOOL InitFMOD();
	virtual void Shutdown();

	virtual FMOD::Sound* RegisterSoundSample( USound* Sound );
	virtual FMOD::Sound* RegisterMusicSample( UMusic* Music );

	virtual UBOOL PlayMusic( UMusic* music, FMOD::Channel*channel, BYTE SongSection, BYTE CdTrack, EMusicTransition Transition );
	virtual void ApplyChannelGroup( FMOD::ChannelGroup* ChannelGroup );
	virtual void SetVolumes();

	virtual void LoadGeometry();
	virtual UBOOL IsObstructed( AActor* Actor );

	virtual void DrawChannelGroup( FMOD::ChannelGroup* group, UCanvas* canvas, UFont* font, float x, float yl, float ym, DWORD detail=-1 );
	virtual FString PrintChannel( FMOD::Channel* channel, FColor* color=NULL );

	void DumpDrivers();
	void DumpPlugins();
	void DumpSystem();
};


#endif
/* ----------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------- */