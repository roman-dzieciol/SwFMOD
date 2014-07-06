/* ============================================================================
	SwFMODCanvas.cpp:
	Copyright 2007 Roman Switch` Dzieciol. All Rights Reserved.
===============================================================================
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.
============================================================================ */



// Includes.
#include "SwFMOD.h"



void USwFMOD::PostRender( FSceneNode* Frame )
{
	guard(USwFMOD::PostRender);
	//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: "), SWF_LOGP );
	FMOD_RESULT result;
	FString text;

	if( !System )
		return;

	int xl, yl;
	int ym = 1;

	UCanvas* canvas = Frame->Viewport->Canvas;
	UFont* statfont = canvas->SmallFont;

	canvas->WrappedStrLenf( statfont, xl, yl, TEXT("0.123456789qwertyQWERTY") );
	canvas->Color = FColor(0,255,0);
	canvas->CurX = 0;
	canvas->CurY = 0;

	// CPU & memory stats
	if( StatGlobal )
	{
		float cpu_dsp;
		float cpu_stream;
		float cpu_update;
		float cpu_total;
		int currentalloced;
		int maxalloced;
		//int currentalloced;
		//int maxalloced;
		//int memtotal;

		//SWF_FMOD_CALL( System->getSoundRAM(&currentalloced,&maxalloced,&memtotal) );
		SWF_FMOD_CALL( FMOD::Memory_GetStats(&currentalloced,&maxalloced) );
		SWF_FMOD_CALL( System->getCPUUsage( &cpu_dsp, &cpu_stream, &cpu_update, &cpu_total ) );

		canvas->Color = FColor(0,224,0);
		canvas->CurX = yl;
		canvas->CurY += yl+ym;
		canvas->WrappedPrintf( statfont, 0
			, TEXT("FMOD CPU - DSP: %05.2f%% Stream: %05.2f%% Update: %05.2f%% Total: %05.2f%%, MEM KB - Current: %s Max: %s")
			, cpu_dsp, cpu_stream, cpu_update, cpu_total, *SwGetBytesString(currentalloced), *SwGetBytesString(maxalloced) );
	}


	// Channels
	if( StatPositions )
	{
		canvas->CurX = yl;
		canvas->CurY += yl+ym;
		FString text = TEXT("Positions:");
		for( int i=0; i<6; ++i )
		{
			FSwPosition pos = MusicPositions(i);
			if( pos.ms != 0 )
			{
				text += FString::Printf( TEXT(" %.02f"), static_cast<FLOAT>(pos.ms*0.001f) );
			}
			else
			{
				text += FString::Printf( TEXT(" %u:%u"), pos.row, pos.pattern );
			}
		}
		canvas->Color = FColor(255,192,0);
		canvas->WrappedPrintf( statfont, 0, TEXT("%s"), text );
	}
	

	// Channel groups
	if( StatChannelGroup )
	{
		// Playing
		int ch_playing;
		SWF_FMOD_CALL( System->getChannelsPlaying(&ch_playing) );

		canvas->Color = FColor(0,255,255);
		canvas->CurX = yl;
		canvas->CurY += yl+ym;
		canvas->WrappedPrintf( statfont, 0, TEXT("Playing: %d"), ch_playing );

		// Channel groups
		DrawChannelGroup( MasterChannel, canvas, statfont, yl, yl, ym, 1 );
	}


	// Channels
	if( StatChannels )
	{
		canvas->CurX = yl;
		canvas->CurY += yl+ym;
		for( FSwChannelEnumerator it(System,MasterChannel); it; ++it )
		{
			FMOD::Channel* channel = *it;
			if( IsChannelValid(channel) )
			{
				text = PrintChannel( channel, &canvas->Color );
				canvas->CurX = yl;
				canvas->WrappedPrintf( statfont, 0, TEXT("%s"), text );
			}
		}
	}

	unguard;
}



void USwFMOD::DrawChannelGroup( FMOD::ChannelGroup* group, UCanvas* canvas, UFont* font, float x, float yl, float ym, DWORD detail )
{
	guard(USwFMOD::DrawChannelGroup);
	FMOD_RESULT result;
	FString text;

	static FColor cdefault = FColor(0,255,255);
	static FColor cmute = FColor(0,192,255);
	static FColor cpause = FColor(0,64,255);

	canvas->Color = cdefault;

	char name[1024];
	bool mute;
	bool paused;
	float pitch;
	float volume;
	int channels;
	int groups;


	SWF_FMOD_CALL( group->getPitch(&pitch) );			text += FString::Printf(TEXT(" Pitch:%.01f"), pitch );
	SWF_FMOD_CALL( group->getVolume(&volume) );			text += FString::Printf(TEXT(" Vol:%.01f"), volume );
	SWF_FMOD_CALL( group->getNumChannels(&channels) );	text += FString::Printf(TEXT(" Chans:%i"), channels );
	SWF_FMOD_CALL( group->getNumGroups(&groups) );		text += FString::Printf(TEXT(" Groups:%i"), groups );
	SWF_FMOD_CALL( group->getName(name,1024) );			text += FString::Printf(TEXT(" %s"), *ToStr(appFromAnsi(name)) );
	SWF_FMOD_CALL( group->getMute(&mute) );				if( mute )		{ text += TEXT(" MUTE");  canvas->Color = cmute; }
	SWF_FMOD_CALL( group->getPaused(&paused) );			if( paused )	{ text += TEXT(" PAUSE"); canvas->Color = cpause; }

	text = text.Mid(1);
	canvas->CurX = x;
	canvas->WrappedPrintf( font, 0, TEXT("%s"), *text );

	if( detail > 0 )
	{
		for( int i=0; i<groups; ++i )
		{
			FMOD::ChannelGroup* child;
			SWF_FMOD_CALL( group->getGroup(i,&child) );
			DrawChannelGroup( child, canvas, font, x+yl*4, yl, ym, detail );
		}
	}

	unguard;
}

FString USwFMOD::PrintChannel( FMOD::Channel* channel, FColor* color )
{
	guard(USwFMOD::PrintChannel);
	FMOD_RESULT result;
	FString text;
	FColor c;

	if( !color )
		color = &c;

	static FColor cdefault = FColor(0,255,0);
	static FColor cmute = FColor(0,192,0);
	static FColor cpause = FColor(0,64,0);
	static FColor cmusic = FColor(255,255,0);
	static FColor cambient = FColor(255,0,255);
	static FColor ccompressed = FColor(255,128,0);
	static FColor c2d = FColor(0,128,255);

	*color = cdefault;


	float audibility;
	unsigned int startdelay, enddelay;
	float frequency;
	bool mute;
	bool paused;
	int priority;
	bool isvirtual;
	bool isplaying;
	int index;
	void* data = NULL;

	SWF_FMOD_CALL( channel->getIndex(&index) );					text += FString::Printf(TEXT(" [%3i]"), index );
	SWF_FMOD_CALL( channel->getPriority(&priority) );			text += FString::Printf(TEXT(" P:%3i"), priority );

	if( IsChannelValid( channel ) )
	{
		SWF_FMOD_CALL( channel->getAudibility(&audibility) );		text += FString::Printf(TEXT(" A:%.01f"), audibility );
		SWF_FMOD_CALL( channel->getDelay(&startdelay, &enddelay) );
		SWF_FMOD_CALL( channel->getFrequency(&frequency) );
		SWF_FMOD_CALL( channel->isVirtual(&isvirtual) );
		SWF_FMOD_CALL( channel->isPlaying(&isplaying) );
		SWF_FMOD_CALL( channel->getUserData(&data) );	
		

		FMOD::Sound* sample = GetChannelSample(channel);
		if( !sample )
			return text;

		UObject* object = GetSampleObject(sample);
		if( !object )
			return text;
		
		USound* sound = Cast<USound>(object);
		UMusic* music = Cast<UMusic>(object);

		FSwSoundId id = GetChannelId(channel);
		AActor* actor = id.GetActor();
		text += FString::Printf(TEXT(" [%s]"), *ToStr(id) );


		// Get defaults
		FLOAT deffrequency;
		FLOAT defvolume;
		FLOAT defpanning;
		INT defpriority;
		SWF_FMOD_CALL( sample->getDefaults( &deffrequency, &defvolume, &defpanning, &defpriority ) );

		// Get flags
		FMOD_MODE fmode;
		SWF_FMOD_CALL( channel->getMode(&fmode) );


		if( music )
		{
			*color = cmusic;

			text += FString::Printf(TEXT(" [%s]"), *music->FileType );
			text += FString::Printf(TEXT(" [%s]"), music->GetName() );
			
			// FIXME:: Doesn't work with volume 0
			UINT pos = 0;
			channel->getPosition(&pos,FMOD_TIMEUNIT_MODORDER);
			
			text += FString::Printf(TEXT(" Section:%u"), pos );
			//text += FString::Printf(TEXT(" CdTrack:%i"), int(p->Music->CdTrack) );
			text += FString::Printf(TEXT(" Transition:%i"), int(Viewport->Actor->Transition) );
		}
		else if( sound )
		{
			// USound
			text += FString::Printf(TEXT(" [%s]"), *sound->FileType );
			text += FString::Printf(TEXT(" [%s]"), sound->GetName() );

			if( sound->FileType != SoundType_WAV )
			{
				*color = ccompressed;
			}

			// sound effect
			if( actor )
			{
				// Actor
				text += FString::Printf(TEXT(" [%s]"), actor->GetName() );
			}

			if( id.GetSlot() == SLOT_Ambient )
			{
				*color = cambient;
			}

			if( !HasFlag(fmode,FMOD_3D) || HasFlag(fmode,FMOD_2D) )
			{
				*color = c2d;
			}
		}

		SWF_FMOD_CALL( channel->getMute(&mute) );					if( mute )		{ text += TEXT(" MUTE");  *color = cmute; }
		SWF_FMOD_CALL( channel->getPaused(&paused) );				if( paused )	{ text += TEXT(" PAUSE"); *color = cpause; }

		if( isvirtual )
		{
			color->R *= 0.66;
			color->G *= 0.66;
			color->B *= 0.66;
		}
	}
	else
	{
		text += TEXT(" INVALID");
	}


	return text.Mid(1);
	unguard;
}


UBOOL USwFMOD::Exec( const TCHAR* Cmd, FOutputDevice& Ar )
{
	guard(USwFMOD::Exec);
	//SWF_LOG( NAME_DevSound, TEXT("%s -- %s :: "), SWF_LOGP );

	const TCHAR* Str = Cmd;
	if( ParseCommand(&Str,TEXT("ASTAT")) )
	{
		if( ParseCommand(&Str,TEXT("Global")) )
		{
			StatGlobal ^= 1;
			return 1;
		}
		if( ParseCommand(&Str,TEXT("ChannelGroup")) )
		{
			StatChannelGroup ^= 1;
			return 1;
		}
		if( ParseCommand(&Str,TEXT("Channels")) )
		{
			StatChannels ^= 1;
			return 1;
		}
		if( ParseCommand(&Str,TEXT("StatRender")) )
		{
			StatRender ^= 1;
			return 1;
		}
	}

	return 0;
	unguard;
}



/* ----------------------------------------------------------------------------
	The End.
---------------------------------------------------------------------------- */
