/* ============================================================================
	SwFMOD.cpp:
	Copyright 2007 Roman Switch` Dzieciol. All Rights Reserved.
===============================================================================
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
============================================================================ */

// FMOD Includes.
#pragma pack(push)
#pragma pack(8)
//#include "fmod_errors.h"
#pragma pack(pop)

// Includes.
#include "SwFMOD.h"


/* ----------------------------------------------------------------------------
	UE glue
---------------------------------------------------------------------------- */

// Names
SWFMOD_API FName SWFMOD_SwFMOD = FName(TEXT("SwFMOD"),FNAME_Intrinsic);
SWFMOD_API FName SWFMOD_SwFMOD_WARNING = FName(TEXT("SwFMOD_WARNING"),FNAME_Intrinsic);

// Package implementation.
IMPLEMENT_PACKAGE(SwFMOD);

// Register USwFMOD.
IMPLEMENT_CLASS(USwFMOD);


/*------------------------------------------------------------------------------------
	Globals
------------------------------------------------------------------------------------*/

SWFMOD_API FLOAT GUUToMeters = 0.02f;
SWFMOD_API DWORD GSwfErrorCount = 0;


/*------------------------------------------------------------------------------------
	USwFMOD :: static
------------------------------------------------------------------------------------*/

const FName USwFMOD::SoundType_WAV = FName(TEXT("wav"),FNAME_Intrinsic);
const FName USwFMOD::SoundType_MP2 = FName(TEXT("mp2"),FNAME_Intrinsic);
const FName USwFMOD::SoundType_MP3 = FName(TEXT("mp3"),FNAME_Intrinsic);
const FName USwFMOD::SoundType_OGG = FName(TEXT("ogg"),FNAME_Intrinsic);


const DWORD USwFMOD::OutputRates[] = 
{	8000
,	11025
,	16000
,	22050
,	32000
,	44100
,	48000
};


/*------------------------------------------------------------------------------------
	USwFMOD.
------------------------------------------------------------------------------------*/

USwFMOD::USwFMOD()
{
	guard(USwFMOD::USwFMOD);
	SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: "), SWF_PLOG );
	unguard;
}

void USwFMOD::StaticConstructor()
{
	guard(USwFMOD::StaticConstructor);
	SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: "), SWF_PLOG );
	
	// Internals
	bInitialized		= 0;
	Viewport			= NULL;
	
	MusicFade			= -1;
	MusicFadeTime		= -1;
	LastTime			= appSeconds();

	// FMOD
	System				= NULL;
	AmbientChannels 	= NULL;
	EffectChannels		= NULL;
	MusicChannels		= NULL;
	CompressedChannels	= NULL;
	MasterChannel		= NULL;
	MusicGroup			= NULL;
	MasterGroup			= NULL;
	Geometry			= NULL;
	
	
	// Defaults
	VolumeMusic			= 255.0f;
	VolumeSound			= 255.0f;
	VolumeSpeech		= 255.0f;
	AmbientFactor		= 0.7f;
	AmbientHysteresis	= 256.0f;

	SampleRate			= 6;
	SampleFormat		= 4;
	SampleInterpolation	= 3;

	b3DCameraSounds		= 1;
	bHRTF				= 1;
	bLowSoundQuality	= 0;
	bOcclusion			= 0;

	Channels			= 64;
	VirtualChannels		= 64;
	VirtualThreshold	= 0.00f;

	PriorityMusic		= 0;
	PrioritySound		= 255;
	PrioritySpeech		= 127;
	PriorityAmbient		= 192;

	DistanceMin			= 50.0f;
	ToMeters			= 0.02f;

	StatGlobal			= 0;
	StatChannelGroup	= 0;
	StatChannels		= 0;
	StatRender			= 0;
	StatPositions		= 0;

	bLogPlugins			= 0;
	bPrecache			= 1;

	DopplerScale		= 1.0f;
	DistanceFactor		= 1.0f;
	RolloffScale		= 1.0f;

	HRTFMinAngle		= 180.0f;
	HRTFMaxAngle		= 360.0f;
	HRTFFreq			= 4000.0f;


	OverrideOutput			= -1;
	OverrideOutputChannels	= -1;
	OverrideInputChannels	= 2;
	OverrideDSPBufferLength	= -1;
	OverrideDSPBufferCount	= -1;
	OverrideInitFlags		= -1;
	OverrideDebugFlags		= -1;
	OverrideSpeakerMode		= -1;

	Driver			= 0;
	Min2D			= 0;
	Max2D			= 0;
	Min3D			= 0;
	Max3D			= 0;

	MusicPositions.Empty();
	MusicPositions.InsertZeroed(0,256);


	UEnum* EOutputRates = new( GetClass(), TEXT("EOutputRates") )UEnum( NULL );
		new( EOutputRates->Names )FName( TEXT("8000Hz" ) );
		new( EOutputRates->Names )FName( TEXT("11025Hz") );
		new( EOutputRates->Names )FName( TEXT("16000Hz") );
		new( EOutputRates->Names )FName( TEXT("22050Hz") );
		new( EOutputRates->Names )FName( TEXT("32000Hz") );
		new( EOutputRates->Names )FName( TEXT("44100Hz") );
		new( EOutputRates->Names )FName( TEXT("48000Hz") );
		
	UEnum* ESoundFormats = new( GetClass(), TEXT("ESoundFormats") )UEnum( NULL );
		new( ESoundFormats->Names )FName( TEXT("PCM8" ) );
		new( ESoundFormats->Names )FName( TEXT("PCM16") );
		new( ESoundFormats->Names )FName( TEXT("PCM24") );
		new( ESoundFormats->Names )FName( TEXT("PCM32") );
		new( ESoundFormats->Names )FName( TEXT("PCMFLOAT") );
		
	UEnum* EResamplers = new( GetClass(), TEXT("EResamplers") )UEnum( NULL );
		new( EResamplers->Names )FName( TEXT("No" ) );
		new( EResamplers->Names )FName( TEXT("Linear") );
		new( EResamplers->Names )FName( TEXT("Cubic") );
		new( EResamplers->Names )FName( TEXT("Spline") );

	// TODO:: figure out why UBoolProperty config doesn't work properly

	new(GetClass(),TEXT("bLogPlugins"),			RF_Public)UIntProperty (CPP_PROPERTY(bLogPlugins),					TEXT("SwFMOD"), CPF_Config );

	new(GetClass(),TEXT("StatGlobal"),					RF_Public)UIntProperty (CPP_PROPERTY(StatGlobal),					TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("StatChannelGroup"),			RF_Public)UIntProperty (CPP_PROPERTY(StatChannelGroup),				TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("StatChannels"),				RF_Public)UIntProperty (CPP_PROPERTY(StatChannels),					TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("StatRender"),					RF_Public)UIntProperty (CPP_PROPERTY(StatRender),					TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("StatPositions"),				RF_Public)UIntProperty (CPP_PROPERTY(StatPositions),					TEXT("SwFMOD"), CPF_Config );

	new(GetClass(),TEXT("bPrecache"),			RF_Public)UIntProperty (CPP_PROPERTY(bPrecache),			TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("b3DCameraSounds"),		RF_Public)UIntProperty (CPP_PROPERTY(b3DCameraSounds),		TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("LowSoundQuality"),		RF_Public)UIntProperty (CPP_PROPERTY(bLowSoundQuality),		TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("bOcclusion"),			RF_Public)UIntProperty (CPP_PROPERTY(bOcclusion),			TEXT("SwFMOD"), CPF_Config );

	new(GetClass(),TEXT("Driver"),						RF_Public)UIntProperty (CPP_PROPERTY(Driver),				TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("Min2D"),						RF_Public)UIntProperty (CPP_PROPERTY(Min2D),				TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("Max2D"),						RF_Public)UIntProperty (CPP_PROPERTY(Max2D),				TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("Min3D"),						RF_Public)UIntProperty (CPP_PROPERTY(Min3D),				TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("Max3D"),						RF_Public)UIntProperty (CPP_PROPERTY(Max3D),				TEXT("SwFMOD"), CPF_Config );

	new(GetClass(),TEXT("OverrideOutput"),				RF_Public)UIntProperty (CPP_PROPERTY(OverrideOutput),				TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("OverrideOutputChannels"),		RF_Public)UIntProperty (CPP_PROPERTY(OverrideOutputChannels),		TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("OverrideInputChannels"),		RF_Public)UIntProperty (CPP_PROPERTY(OverrideInputChannels),		TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("OverrideDSPBufferLength"),		RF_Public)UIntProperty (CPP_PROPERTY(OverrideDSPBufferLength),		TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("OverrideDSPBufferCount"),		RF_Public)UIntProperty (CPP_PROPERTY(OverrideDSPBufferCount),		TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("OverrideInitFlags"),			RF_Public)UIntProperty (CPP_PROPERTY(OverrideInitFlags),			TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("OverrideDebugFlags"),			RF_Public)UIntProperty (CPP_PROPERTY(OverrideDebugFlags),			TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("OverrideSpeakerMode"),			RF_Public)UIntProperty (CPP_PROPERTY(OverrideSpeakerMode),			TEXT("SwFMOD"), CPF_Config );
	
	new(GetClass(),TEXT("ToMeters"),			RF_Public)UFloatProperty (CPP_PROPERTY(ToMeters),				TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("DopplerScale"),		RF_Public)UFloatProperty (CPP_PROPERTY(DopplerScale),			TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("DistanceMin"),			RF_Public)UFloatProperty (CPP_PROPERTY(DistanceMin),			TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("DistanceFactor"),		RF_Public)UFloatProperty (CPP_PROPERTY(DistanceFactor),			TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("RolloffScale"),		RF_Public)UFloatProperty (CPP_PROPERTY(RolloffScale),			TEXT("SwFMOD"), CPF_Config );

	new(GetClass(),TEXT("bHRTF"),				RF_Public)UIntProperty (CPP_PROPERTY(bHRTF),					TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("HRTFMinAngle"),		RF_Public)UFloatProperty (CPP_PROPERTY(HRTFMinAngle),			TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("HRTFMaxAngle"),		RF_Public)UFloatProperty (CPP_PROPERTY(HRTFMaxAngle),			TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("HRTFFreq"),			RF_Public)UFloatProperty (CPP_PROPERTY(HRTFFreq),				TEXT("SwFMOD"), CPF_Config );

	new(GetClass(),TEXT("PriorityMusic"),		RF_Public)UByteProperty (CPP_PROPERTY(PriorityMusic),			TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("PrioritySound"),		RF_Public)UByteProperty (CPP_PROPERTY(PrioritySound),			TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("PrioritySpeech"),		RF_Public)UByteProperty (CPP_PROPERTY(PrioritySpeech),			TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("PriorityAmbient"),		RF_Public)UByteProperty (CPP_PROPERTY(PriorityAmbient),			TEXT("SwFMOD"), CPF_Config );

	new(GetClass(),TEXT("Channels"),			RF_Public)UIntProperty (CPP_PROPERTY(Channels),					TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("VirtualChannels"),		RF_Public)UIntProperty (CPP_PROPERTY(VirtualChannels),			TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("VirtualThreshold"),	RF_Public)UFloatProperty (CPP_PROPERTY(VirtualThreshold),		TEXT("SwFMOD"), CPF_Config );
	
	new(GetClass(),TEXT("SampleRate"),				RF_Public)UByteProperty	(CPP_PROPERTY(SampleRate),				TEXT("SwFMOD"), CPF_Config, EOutputRates );
	new(GetClass(),TEXT("SampleFormat"),			RF_Public)UByteProperty	(CPP_PROPERTY(SampleFormat),			TEXT("SwFMOD"), CPF_Config, ESoundFormats );
	new(GetClass(),TEXT("SampleInterpolation"),		RF_Public)UByteProperty	(CPP_PROPERTY(SampleInterpolation),		TEXT("SwFMOD"), CPF_Config, EResamplers );

	new(GetClass(),TEXT("AmbientHysteresis"),	RF_Public)UFloatProperty (CPP_PROPERTY(AmbientHysteresis),		TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("AmbientFactor"),		RF_Public)UFloatProperty (CPP_PROPERTY(AmbientFactor),			TEXT("SwFMOD"), CPF_Config );

	new(GetClass(),TEXT("MusicVolume"),			RF_Public)UFloatProperty (CPP_PROPERTY(VolumeMusic),			TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("SpeechVolume"),		RF_Public)UFloatProperty (CPP_PROPERTY(VolumeSpeech),			TEXT("SwFMOD"), CPF_Config );
	new(GetClass(),TEXT("SoundVolume"),			RF_Public)UFloatProperty (CPP_PROPERTY(VolumeSound),			TEXT("SwFMOD"), CPF_Config );


	VerifyConfig();
	GUUToMeters = ToMeters;

	unguard;
}

void USwFMOD::VerifyConfig()
{
	guard(USwFMOD::VerifyConfig);
	SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: "), SWF_PLOG );

	VolumeMusic			= Clamp(VolumeMusic,0.0f,255.0f);
	VolumeSound			= Clamp(VolumeSound,0.0f,255.0f);
	VolumeSpeech		= Clamp(VolumeSpeech,0.0f,255.0f);
	AmbientFactor		= Clamp(AmbientFactor,0.0f,10.0f);

	SampleRate				= Clamp(SampleRate,(BYTE)0,(BYTE)6);
	SampleFormat			= Clamp(SampleFormat,(BYTE)0,(BYTE)(FMOD_SOUND_FORMAT_MAX-1));
	SampleInterpolation		= Clamp(SampleInterpolation,(BYTE)0,(BYTE)(FMOD_DSP_RESAMPLER_MAX-1));

	Channels			= Clamp(Channels,(DWORD)0,(DWORD)255);
	VirtualChannels		= Clamp(VirtualChannels,(DWORD)0,(DWORD)4096);
	VirtualThreshold	= Clamp(VirtualThreshold,0.0f,1.0f);
		
	PriorityMusic		= Clamp(PriorityMusic,(BYTE)0,(BYTE)255);
	PrioritySound		= Clamp(PrioritySound,(BYTE)0,(BYTE)255);
	PrioritySpeech		= Clamp(PrioritySpeech,(BYTE)0,(BYTE)255);
	PriorityAmbient		= Clamp(PriorityAmbient,(BYTE)0,(BYTE)255);

	DopplerScale		= Clamp(DopplerScale,0.0f,10.0f);
	DistanceFactor		= Clamp(DistanceFactor,0.0f,10.0f);
	RolloffScale		= Clamp(RolloffScale,0.0f,10.0f);

	HRTFMinAngle		= Clamp(HRTFMinAngle,0.0f,360.0f);
	HRTFMaxAngle		= Clamp(HRTFMaxAngle,0.0f,360.0f);
	HRTFFreq			= Clamp(HRTFFreq,0.0f,20000.0f);

	Driver		= Clamp(Driver,(INT)0,(INT)255);

	if( ToMeters <= 0 )
		ToMeters = 0.02f;
	
	if( DistanceMin <= 0 )
		DistanceMin = 1.0f;


	unguard;
}


/*------------------------------------------------------------------------------------
	UObject Interface.
------------------------------------------------------------------------------------*/

void USwFMOD::PostEditChange()
{
	guard(USwFMOD::PostEditChange);
	SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: "), SWF_PLOG );

	VerifyConfig();
	SetVolumes();

	unguard;
}

void USwFMOD::Destroy()
{
	guard(USwFMOD::Destroy);
	SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: "), SWF_PLOG );

	if( bInitialized )
	{
		// Unhook.
		USound::Audio = NULL;
		UMusic::Audio = NULL;
	}

	// Shut down viewport.
	SetViewport( NULL );

	// Shutdown soundsystem.
	Shutdown();

	SWF_LOG( NAME_Exit, TEXT("SwFMOD audio subsystem shut down.") );
	Super::Destroy();
	unguard;
}

void USwFMOD::ShutdownAfterError()
{
	guard(USwFMOD::ShutdownAfterError);
	SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: "), SWF_PLOG );

	// Unhook.
	USound::Audio = NULL;
	UMusic::Audio = NULL;

	// Safely shut down.
	SWF_LOG( NAME_Exit, TEXT("USwFMOD::ShutdownAfterError") );

	// Shutdown soundsystem.
	Shutdown();

	Super::ShutdownAfterError();
	unguard;
}


/*------------------------------------------------------------------------------------
	UAudioSubsystem Interface.
------------------------------------------------------------------------------------*/

UBOOL USwFMOD::Init()
{
	guard(USwFMOD::Init);
	SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: "), SWF_PLOG );
	
	VerifyConfig();

	if( InitFMOD() )
	{
		// Initialized!
		bInitialized = 1;
		USound::Audio = this;
		UMusic::Audio = this;

		SWF_LOG( NAME_Init, TEXT("SwFMOD audio subsystem initialized.") );
		return 1;
	}

	return 0;
	unguard;
}

UBOOL USwFMOD::InitFMOD()
{
	guard(USwFMOD::InitFMOD);
	SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: "), SWF_PLOG );
	FMOD_RESULT result;

	// verify reinterpret_cast
	if( sizeof(FSwSoundId) > sizeof(void*) )
		return 0;


	//
	// Properties
	//
#pragma pack(push)
#pragma pack(8)
	FMOD_ADVANCEDSETTINGS adv;
	appMemset(&adv, 0, sizeof(FMOD_ADVANCEDSETTINGS));
	adv.cbsize = sizeof(FMOD_ADVANCEDSETTINGS);
#pragma pack(pop)

	INT driver;
	FMOD_OUTPUTTYPE output;
	INT schannels;
	INT vchannels;
	INT rate;
	FMOD_SOUND_FORMAT format;
	INT numoutputchannels;
	INT maxinputchannels;
	FMOD_DSP_RESAMPLER resampler;
	FMOD_SPEAKERMODE speakermode;
	UINT version;
	
	char drivername[1024];
	FMOD_CAPS caps;
	INT minfreq, maxfreq;
	FMOD_SPEAKERMODE driverspeakermode;

	// Flags
	FMOD_INITFLAGS initflags = FMOD_INIT_NORMAL;
	INT debugflags  = FMOD_DEBUG_LEVEL_NONE
	//	| FMOD_DEBUG_LEVEL_LOG
		| FMOD_DEBUG_LEVEL_ERROR
		| FMOD_DEBUG_LEVEL_WARNING
		| FMOD_DEBUG_LEVEL_HINT
	//	| FMOD_DEBUG_LEVEL_ALL
	//	| FMOD_DEBUG_TYPE_MEMORY
	//	| FMOD_DEBUG_TYPE_THREAD
	//	| FMOD_DEBUG_TYPE_FILE
		| FMOD_DEBUG_TYPE_NET
		| FMOD_DEBUG_TYPE_EVENT
	//	| FMOD_DEBUG_TYPE_ALL
		| FMOD_DEBUG_DISPLAY_TIMESTAMPS
		| FMOD_DEBUG_DISPLAY_LINENUMBERS
		| FMOD_DEBUG_DISPLAY_COMPRESS
		| FMOD_DEBUG_DISPLAY_ALL
	//	| FMOD_DEBUG_ALL
		;


	//
	// config
	//
	guard(Config);

	output = FMOD_OUTPUTTYPE_AUTODETECT;
	driver = Driver;
	schannels = Channels;
	vchannels = VirtualChannels;
	rate = OutputRates[SampleRate];
	format = static_cast<FMOD_SOUND_FORMAT>(SampleFormat);
	resampler = static_cast<FMOD_DSP_RESAMPLER>(SampleInterpolation);
	numoutputchannels = 0;
	maxinputchannels = 6;

	if( bHRTF )
	{
		initflags |= FMOD_INIT_SOFTWARE_HRTF;
	}

	if( bOcclusion )
	{
		initflags |= FMOD_INIT_SOFTWARE_OCCLUSION;
	}

	if( VirtualThreshold > -1 )
	{
		initflags |= FMOD_INIT_VOL0_BECOMES_VIRTUAL;
	}

	if( bLowSoundQuality )
	{
		rate = 44100; // low rate
		format = FMOD_SOUND_FORMAT_PCM16; // default format
		resampler = FMOD_DSP_RESAMPLER_LINEAR; // fast mixer
		schannels = 16; // few channels
		vchannels = 16; // few virtual channels
		initflags &= ~FMOD_INIT_SOFTWARE_HRTF; // no HRTF
		initflags &= ~FMOD_INIT_SOFTWARE_OCCLUSION; // no occlusion
		initflags &= ~FMOD_INIT_VOL0_BECOMES_VIRTUAL; // no premature virtuals
	}

	unguard;


	//
	// pre-init
	//
	guard(PreInit);

	// Create the main system object.
	SWF_FMOD_RCALL0( FMOD::System_Create(&System) );
	
	// Debug flags, ignore errors
	TSwOverride( debugflags, OverrideDebugFlags );
	FMOD::Debug_SetLevel(debugflags);

	// Verify version
	SWF_FMOD_RCALL0( System->getVersion(&version) );
	if( version < FMOD_VERSION )
	{
		SWF_LOG( SWF_NAME, TEXT("You are using an old version of FMOD %08x.  This program requires %08x\n"), version, FMOD_VERSION );
		return 0;
	}

	// Dump defaults
	//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: Default FMOD System properties"), SWF_PLOG );
	//DumpSystem();


	// Dump drivers
	DumpDrivers();

	// Verify driver
	INT numdrivers;
	SWF_FMOD_RCALL0( System->getNumDrivers(&numdrivers) );
	if( driver >= numdrivers )
		driver = 0;

	// Set Driver
	SWF_FMOD_RCALL0( System->getDriverName( driver, drivername, 1024) );
	SWF_LOG( SWF_NAME, TEXT("%s -- %s :: Using driver: %s"), SWF_PLOG, *ToStr(appFromAnsi(drivername)) );
	SWF_FMOD_RCALL0( System->setDriver( driver ) );

	// Use driver speaker mode
	SWF_FMOD_RCALL0( System->getDriverCaps( driver, &caps, &minfreq, &maxfreq, &driverspeakermode ) );
	speakermode = driverspeakermode;


	// Dump plugins
	if( bLogPlugins )
		DumpPlugins();


	// Output device
	TSwOverride(output,OverrideOutput);
	SWF_FMOD_RCALL0( System->setOutput(output) );

	// Speaker mode
	TSwOverride(speakermode,OverrideSpeakerMode);
	SWF_FMOD_RCALL0( System->setSpeakerMode(speakermode) );

	// Software channels
	SWF_FMOD_RCALL0( System->setSoftwareChannels(schannels) );

	// Disable hardware
	SWF_FMOD_RCALL0( System->setHardwareChannels(Min2D, Max2D, Min3D, Max3D) );

	// Mixer settings
	TSwOverride(numoutputchannels,OverrideOutputChannels);
	TSwOverride(maxinputchannels,OverrideInputChannels);
	SWF_FMOD_RCALL0( System->setSoftwareFormat( rate, format, numoutputchannels, maxinputchannels, resampler ) );

	// DSP buffer
	if( OverrideDSPBufferLength != -1 && OverrideDSPBufferCount != -1 )
		System->setDSPBufferSize( OverrideDSPBufferLength, OverrideDSPBufferCount );

	// Advanced options
	SWF_FMOD_RCALL0( System->getAdvancedSettings(&adv) );
	if( HasFlag(initflags,FMOD_INIT_VOL0_BECOMES_VIRTUAL) )
	{
		adv.vol0virtualvol = VirtualThreshold;
	}
	if( HasFlag(initflags,FMOD_INIT_SOFTWARE_HRTF) )
	{
		adv.HRTFMinAngle = HRTFMinAngle;
		adv.HRTFMaxAngle = HRTFMaxAngle;
		adv.HRTFFreq = HRTFFreq;
	}
	SWF_FMOD_RCALL0( System->setAdvancedSettings(&adv) );

	unguard;

	// Dump defaults
	//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: FMOD pre-init system properties"), SWF_PLOG );
	//DumpSystem();

	
	//
	// Initialize system object
	//
	guard(Init);
	SWF_FMOD_RCALL0( System->init(schannels+vchannels, initflags, 0) );
	unguard;


	//
	// post-init
	//
	guard(PostInit);

	// Distance model
	SWF_FMOD_RCALL0( System->set3DSettings(DopplerScale, DistanceFactor, RolloffScale ));

	// Create Channel Groups
	SWF_FMOD_RCALL0( System->getMasterChannelGroup(&MasterChannel) );

	SWF_FMOD_RCALL0( System->createChannelGroup( "Music",&MusicChannels) );
	SWF_FMOD_RCALL0( System->createChannelGroup( "Compressed",&CompressedChannels) );
	SWF_FMOD_RCALL0( System->createChannelGroup( "Ambient",&AmbientChannels) );
	SWF_FMOD_RCALL0( System->createChannelGroup( "Effects",&EffectChannels) );

	SWF_FMOD_RCALL0( MasterChannel->addGroup(MusicChannels) );
	SWF_FMOD_RCALL0( MasterChannel->addGroup(CompressedChannels) );
	SWF_FMOD_RCALL0( MasterChannel->addGroup(AmbientChannels) );
	SWF_FMOD_RCALL0( MasterChannel->addGroup(EffectChannels) );

	// Create Sound Groups
	//SWF_FMOD_RCALL0( System->getMasterSoundGroup(&MasterGroup) );
	//SWF_FMOD_RCALL0( System->createSoundGroup( "Music",&MusicGroup) );

	// Dump defaults
	SWF_LOG_DEV( TEXT("FMOD post-init system properties") );
	DumpSystem();

	unguard;

	return 1;
	unguard;
}


void USwFMOD::Shutdown()
{
	guard(USwFMOD::Shutdown);
	SWF_LOG( NAME_DevSound, TEXT("%s -- %s ::"), SWF_PLOG );
	FMOD_RESULT result;

	if( System )
	{
		// RELEASE ALL FMOD OBJECTS !!!

		// Stop channels
		for( FSwChannelEnumerator it(System); it; ++it )
		{
			FMOD::Channel* channel = *it;
			if( IsChannelValid(channel) )
			{
				SWF_FMOD_CALL( channel->setUserData(NULL) );
				SWF_FMOD_CALL( channel->stop() );
			}
		}

		// Release channel groups
		SWF_FMOD_CALL( AmbientChannels->release() );
		SWF_FMOD_CALL( EffectChannels->release() );
		SWF_FMOD_CALL( MusicChannels->release() );
		SWF_FMOD_CALL( CompressedChannels->release() );


		// Release sound groups
		//SWF_FMOD_CALL( MusicGroup->release() );

		// Release geometry
		if( Geometry )
		{
			SWF_FMOD_CALL( Geometry->setActive(false) );
			SWF_FMOD_CALL( Geometry->release() );
			Geometry = NULL;
		}

		// Release music
		for( TObjectIterator<UMusic> MusicIt; MusicIt; ++MusicIt )
			if( MusicIt->Handle )
				UnregisterMusic( *MusicIt );
			
		// Release sounds
		for( TObjectIterator<USound> SoundIt; SoundIt; ++SoundIt )
			if( SoundIt->Handle )
				UnregisterSound( *SoundIt );

		// Release system
		SWF_FMOD_CALL( System->release() );
		System = NULL;
	}

	SWF_LOG( SWF_NAME, TEXT("Errors: %u"), GSwfErrorCount );
	bInitialized = 0;
	unguard;
}


UBOOL USwFMOD::GetLowQualitySetting()
{
	guard(USwFMOD::GetLowQualitySetting);
	//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: "), SWF_PLOG );
	return bLowSoundQuality;
	unguard;
}

UViewport* USwFMOD::GetViewport()
{
	guard(USwFMOD::GetViewport);

	return Viewport;

	unguard;
}

void USwFMOD::SetViewport( UViewport* InViewport )
{
	guard(USwFMOD::SetViewport);
	SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: [%s],[%s]"), SWF_PLOG, *ToStr(InViewport), *ToStr(Viewport) );
	FMOD_RESULT result;

	// Stop sounds
	if( System )
	{
		for( FSwChannelEnumerator it(System); it; ++it )
		{
			FMOD::Channel* channel = *it;
			if( IsChannelValid(channel) )
			{
				SWF_FMOD_CALL( channel->setUserData(NULL) );
				SWF_FMOD_CALL( channel->stop() );
			}
		}
	}

	// Reset music positions
	MusicPositions.Empty();
	MusicPositions.InsertZeroed(0,256);

	// Remember the viewport.
	if( Viewport != InViewport )
	{
		if( Viewport )
		{
			// Unregister music
			for( TObjectIterator<UMusic> MusicIt; MusicIt; ++MusicIt )
				if( MusicIt->Handle )
					UnregisterMusic( *MusicIt );
				
			// Unregister sounds
			for( TObjectIterator<USound> SoundIt; SoundIt; ++SoundIt )
				if( SoundIt->Handle )
					UnregisterSound( *SoundIt );
		}

		Viewport = InViewport;

		if( Viewport )
		{
			// Register sounds
			if( bPrecache )
			{
				SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: Precaching audio"), SWF_PLOG );
				
				if( Viewport->Actor->Song && !Viewport->Actor->Song->Handle )
						RegisterMusic( Viewport->Actor->Song );

				for( TObjectIterator<USound> SoundIt; SoundIt; ++SoundIt )
					if( !SoundIt->Handle )
						RegisterSound( *SoundIt );

				SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: Precaching done"), SWF_PLOG );
			}

			// Start music
			if( Viewport->Actor->Song && Viewport->Actor->Transition == MTRAN_None )
			{
				Viewport->Actor->Transition = MTRAN_Instant;
			}
				
			// Update volumes
			SetVolumes();
		}
	}

	// Update system
	if( System )
	{
		guard(UpdateFMOD);
		SWF_FMOD_CALL( System->update() );
		unguard;
	}

	unguard;
}



/* ----------------------------------------------------------------------------
	Volume
---------------------------------------------------------------------------- */

void USwFMOD::SetInstantSoundVolume( BYTE NewVolume )
{
	guard(USwFMOD::SetInstantSoundVolume);
	//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: [%s]"), SWF_PLOG, *ToStr(NewVolume) );
	FMOD_RESULT result;

	// Effects 
	VolumeSound = NewVolume;
	FLOAT volume = Clamp(VolumeSound / 255.0f, 0.0f, 1.0f);
	SWF_FMOD_CALL( EffectChannels->setVolume(volume) );

	// Ambient
	AmbientFactor = Clamp(AmbientFactor, 0.0f, 1.0f);
	volume = Clamp(volume*AmbientFactor, 0.0f, 1.0f);
	SWF_FMOD_CALL( AmbientChannels->setVolume(volume) );

	unguard;
}

void USwFMOD::SetInstantSpeechVolume( BYTE NewVolume )
{
	guard(USwFMOD::SetInstantSpeechVolume);
	//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: [%s]"), SWF_PLOG, *ToStr(NewVolume) );
	FMOD_RESULT result;

	// Speech
	VolumeSpeech = NewVolume;
	FLOAT volume = Clamp(VolumeSpeech / 255.0f, 0.0f, 1.0f);
	SWF_FMOD_CALL( CompressedChannels->setVolume(volume) );
	unguard;
}

void USwFMOD::SetInstantMusicVolume( BYTE NewVolume )
{
	guard(USwFMOD::SetInstantMusicVolume);
	//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: [%s]"), SWF_PLOG, *ToStr(NewVolume) );
	FMOD_RESULT result;

	// Music
	VolumeMusic = NewVolume;
	FLOAT volume = Clamp(VolumeMusic / 255.0f, 0.0f, 1.0f);
	SWF_FMOD_CALL( MusicChannels->setVolume(volume) );
	unguard;
}

void USwFMOD::SetVolumes()
{
	guard(USwFMOD::SetVolumes);
	//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: "), SWF_PLOG );
	FMOD_RESULT result;

	// verify volumes
	VolumeMusic		= Clamp(VolumeMusic, 0.0f, 255.0f);
	VolumeSpeech	= Clamp(VolumeSpeech, 0.0f, 255.0f);
	VolumeSound		= Clamp(VolumeSound, 0.0f, 255.0f);
	AmbientFactor	= Clamp(AmbientFactor, 0.0f, 1.0f);

	// translate
	FLOAT mvolume = Clamp(VolumeMusic / 255.0f, 0.0f, 1.0f);
	FLOAT cvolume = Clamp(VolumeSpeech / 255.0f, 0.0f, 1.0f);
	FLOAT svolume = Clamp(VolumeSound / 255.0f, 0.0f, 1.0f);

	// apply
	SWF_FMOD_CALL( MusicChannels->setVolume(mvolume) );
	SWF_FMOD_CALL( CompressedChannels->setVolume(cvolume) );
	SWF_FMOD_CALL( AmbientChannels->setVolume(svolume*AmbientFactor) );
	SWF_FMOD_CALL( EffectChannels->setVolume(svolume) );

	unguard;
}


/* ----------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------- */
