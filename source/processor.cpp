//------------------------------------------------------------------------
// Copyright(c) 2021-2022 Olli Vanhoja.
//------------------------------------------------------------------------

#include "processor.h"
#include "process.h"
#include "cids.h"
#include "paramids.h"

#include "base/source/fstreamer.h"
#include "public.sdk/source/vst/vstaudioprocessoralgo.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

namespace MyVst {
using namespace Steinberg;
using namespace Steinberg::Vst;

DemonProProcessor::DemonProProcessor ()
{
    //--- set the wanted controller for our processor
    setControllerClass (kDemonProControllerUID);
}

DemonProProcessor::~DemonProProcessor ()
{}

tresult PLUGIN_API DemonProProcessor::initialize (FUnknown* context)
{
    //---always initialize the parent-------
    tresult result = AudioEffect::initialize (context);
    if (result != kResultOk) {
        return result;
    }

    demon32[0].gain = DEMON0_GAIN_DEFAULT_N;
    demon32[0].pitch = DEMON0_PITCH_DEFAULT_N;
    demon32[0].boostGain = DEMON0_BOOST_GAIN_DEFAULT_N;
    demon32[0].boostFc = DEMON0_BOOST_FC_DEFAULT_N;
    demon32[0].blend = DEMON0_BLEND_DEFAULT_N;
    demon32[1].gain = DEMON1_GAIN_DEFAULT_N;
    demon32[1].pitch = DEMON1_PITCH_DEFAULT_N;
    demon32[1].boostGain = DEMON1_BOOST_GAIN_DEFAULT_N;
    demon32[1].boostFc = DEMON1_BOOST_FC_DEFAULT_N;
    demon32[1].blend = DEMON1_BLEND_DEFAULT_N;
    demon32[2].gain = DEMON2_GAIN_DEFAULT_N;
    demon32[2].pitch = DEMON2_PITCH_DEFAULT_N;
    demon32[2].boostGain = DEMON2_BOOST_GAIN_DEFAULT_N;
    demon32[2].boostFc = DEMON2_BOOST_FC_DEFAULT_N;
    demon32[2].blend = DEMON2_BLEND_DEFAULT_N;
    demon64[0].gain = DEMON0_GAIN_DEFAULT_N;
    demon64[0].pitch = DEMON0_PITCH_DEFAULT_N;
    demon64[0].boostGain = DEMON0_BOOST_GAIN_DEFAULT_N;
    demon64[0].boostFc = DEMON0_BOOST_FC_DEFAULT_N;
    demon64[0].blend = DEMON0_BLEND_DEFAULT_N;
    demon64[1].gain = DEMON1_GAIN_DEFAULT_N;
    demon64[1].pitch = DEMON1_PITCH_DEFAULT_N;
    demon64[1].boostGain = DEMON1_BOOST_GAIN_DEFAULT_N;
    demon64[1].boostFc = DEMON1_BOOST_FC_DEFAULT_N;
    demon64[1].blend = DEMON1_BLEND_DEFAULT_N;
    demon64[2].gain = DEMON2_GAIN_DEFAULT_N;
    demon64[2].pitch = DEMON2_PITCH_DEFAULT_N;
    demon64[2].boostGain = DEMON2_BOOST_GAIN_DEFAULT_N;
    demon64[2].boostFc = DEMON2_BOOST_FC_DEFAULT_N;
    demon64[2].blend = DEMON2_BLEND_DEFAULT_N;

    //--- create Audio IO ------
    addAudioInput(STR16("Mono In"), Steinberg::Vst::SpeakerArr::kMono);
    addAudioOutput(STR16("Mono Out"), Steinberg::Vst::SpeakerArr::kMono);

    /*
     * We want to grab MIDI CC events from the channel 2 so we need to add
     * two channels and the channel 2 will be in index 2.
     * TODO Any needed?
     */
    addEventInput(STR16("Event In"), 2);

    return kResultOk;
}

tresult PLUGIN_API DemonProProcessor::terminate ()
{
    //---do not forget to call parent ------
    return AudioEffect::terminate();
}

tresult PLUGIN_API DemonProProcessor::setActive (TBool state)
{
	//--- called when the Plug-in is enable/disable (On/Off) -----
    if (state) {
        float sampleRate = (float)this->processSetup.sampleRate;

        for (int i = 3; i < 3; i++) {
            demon32[i].reset(sampleRate);
            demon32[i].updateParams();
            demon64[i].reset(sampleRate);
            demon64[i].updateParams();
        }
    }

	return AudioEffect::setActive (state);
}

void DemonProProcessor::handleParamChanges(IParameterChanges* paramChanges)
{
    /*if (data.inputParameterChanges)
    {
        int32 numParamsChanged = data.inputParameterChanges->getParameterCount ();
        for (int32 index = 0; index < numParamsChanged; index++)
        {
            if (auto* paramQueue = data.inputParameterChanges->getParameterData (index))
            {
                Vst::ParamValue value;
                int32 sampleOffset;
                int32 numPoints = paramQueue->getPointCount ();
                switch (paramQueue->getParameterId ())
                {
				}
			}
		}
	}*/
	int32 numParamsChanged = paramChanges->getParameterCount();

    if (numParamsChanged) {
        // for each parameter which are some changes in this audio block:
        for (int32 i = 0; i < numParamsChanged; i++) {
            IParamValueQueue* paramQueue = paramChanges->getParameterData(i);
            if (paramQueue) {
                ParamValue value;
                int32 sampleOffset;
                int32 numPoints = paramQueue->getPointCount();

                switch (paramQueue->getParameterId()) {
                    case kBypassId:
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                            bBypass = value > 0.5f;
                        }
                        break;

                    case kDemon0GainId:
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                            demon32[0].gain = value;
                            demon64[0].gain = value;
                        }
                        break;
                    case kDemon1GainId:
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                            demon32[1].gain = value;
                            demon64[1].gain = value;
                        }
                        break;
                    case kDemon2GainId:
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                            demon32[2].gain = value;
                            demon64[2].gain = value;
                        }
                        break;

                    case kDemon0PitchId:
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                            demon32[0].pitch = value;
                            demon64[0].pitch = value;
                        }
                        break;
                    case kDemon1PitchId:
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                            demon32[1].pitch = value;
                            demon64[1].pitch = value;
                        }
                        break;
                    case kDemon2PitchId:
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                            demon32[2].pitch = value;
                            demon64[2].pitch = value;
                        }
                        break;

                    case kDemon0BoostGainId:
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                            demon32[0].boostGain = value;
                            demon64[0].boostGain = value;
                        }
                        break;
                    case kDemon1BoostGainId:
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                            demon32[1].boostGain = value;
                            demon64[1].boostGain = value;
                        }
                        break;
                    case kDemon2BoostGainId:
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                            demon32[2].boostGain = value;
                            demon64[2].boostGain = value;
                        }
                        break;

                    case kDemon0BoostFcId:
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                            demon32[0].boostFc = value;
                            demon64[0].boostFc = value;
                        }
                        break;
                    case kDemon0BoostFcId:
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                            demon32[1].boostFc = value;
                            demon64[1].boostFc = value;
                        }
                        break;
                    case kDemon2BoostFcId:
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                            demon32[2].boostFc = value;
                            demon64[2].boostFc = value;
                        }
                        break;

                    case kDemon0BlendId:
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                            demon32[0].blend = value;
                            demon64[0].blend = value;
                        }
                        break;
                    case kDemon1BlendId:
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                            demon32[1].blend = value;
                            demon64[1].blend = value;
                        }
                        break;
                    case kDemon2BlendId:
                        if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue) {
                            demon32[2].blend = value;
                            demon64[2].blend = value;
                        }
                        break;
                }
            }
        }

        for (int i = 0; i < 3; i++) {
            demon32[i].updateParams();
            demon64[i].updateParams();
        }
    }
}

tresult PLUGIN_API DemonProProcessor::process (Vst::ProcessData& data)
{
	/*
     * Read inputs parameter changes.
     */
    IParameterChanges* paramChanges = data.inputParameterChanges;
    if (paramChanges) {
        handleParamChanges(paramChanges);
    }

    /*
     * Processing
     */

    if (data.numInputs != 1 || data.numOutputs != 1) {
        return kResultOk;
    }

    void* in = getChannelBuffersPointer(processSetup, data.inputs[0][0]);
    void* out = getChannelBuffersPointer(processSetup, data.outputs[0][0]);
    const int nrSamples = data.numSamples;
    const size_t sampleFramesSize = getSampleFramesSizeInBytes(processSetup, nrSamples);

    if (data.inputs[0].silenceFlags) {
        data.outputs[0].silenceFlags = data.inputs[0].silenceFlags;

        if (in != out) {
            memset(out, 0, sampleFramesSize);
        }
    } else {
        // Normally the output is not silenced
        data.outputs[0].silenceFlags = 0;

        if (bBypass) {
            memmove(out, in, sampleFramesSize);
        } else {
            void *buf;

            buf = malloc(sampleFramesSize);
            if (!buf) {
                return kResultFalse; // TODO Is this the right code?
            }

            memset(out, 0, sampleFramesSize);

            if (data.symbolicSampleSize == kSample32) {
                for (int i = 0; i < 3) {
                    memcpy(buf, in, sampleFramesSize);

                    // TODO Add hardcoded HPF?
                    demon32[i].process((Sample32*)buf, nrSamples);

                    for (int n = 0; n < nrSamples; n++) {
                        ((Sample32*)out)[n] += ((Sample32*)buf)[n] / 3.0f;
                    }
                }
            } else {
                for (int i = 0; i < 3) {
                    memcpy(buf, in, sampleFramesSize);

                    // TODO Add hardcoded HPF?
                    demon64[i].process((Sample64*)buf, nrSamples);

                    for (int n = 0; n < nrSamples; n++) {
                        ((Sample64*)out)[n] += ((Sample64*)buf)[n] / 3.0f;
                    }
                }
            }

            free(buf);
        }
    }

	return kResultOk;
}

tresult PLUGIN_API DemonProProcessor::setupProcessing (Vst::ProcessSetup& newSetup)
{
	//--- called before any processing ----
	return AudioEffect::setupProcessing(newSetup);
}

tresult PLUGIN_API DemonProProcessor::canProcessSampleSize (int32 symbolicSampleSize)
{
	if (symbolicSampleSize == Vst::kSample32 || symbolicSampleSize == Vst::kSample64)
		return kResultTrue;

	return kResultFalse;
}

tresult PLUGIN_API DemonProProcessor::setBusArrangements (Steinberg::Vst::SpeakerArrangement* inputs, int32 numIns,
                                                        Steinberg::Vst::SpeakerArrangement* outputs, int32 numOuts)
{
    if (numIns == 1 && numOuts == 1 &&
        Steinberg::Vst::SpeakerArr::getChannelCount(inputs[0]) == 1 &&
        Steinberg::Vst::SpeakerArr::getChannelCount(outputs[0]) == 1) {
        auto* bus = FCast<Steinberg::Vst::AudioBus>(audioInputs.at (0));
        if (bus) {
            // check if we are Mono => Mono, if not we need to recreate the busses
            if (bus->getArrangement() != inputs[0]) {
                getAudioInput(0)->setArrangement(inputs[0]);
                getAudioInput(0)->setName(STR16("Mono In"));
                getAudioOutput(0)->setArrangement(inputs[0]);
                getAudioOutput(0)->setName (STR16("Mono Out"));
            }
            return kResultOk;
        }
    }
    return kResultFalse;
}

tresult PLUGIN_API DemonProProcessor::setState (IBStream* state)
{
    // called when we load a preset or project, the model has to be reloaded
    if (!state) {
        kResultFalse;
    }

	IBStreamer streamer(state, kLittleEndian);

    int32 savedBypass = 0;

    if (!streamer.readInt32(savedBypass)) {
            return kResultFalse;
    }

    bBypass = savedBypass > 0;

    float sampleRate = (float)this->processSetup.sampleRate;

    for (int i = 0; i < 3; i++) {
        float gain;
        float pitch;
        float boostGain;
        float boostFc;
        float blend;

        if (!streamer.readFloat(gain) ||
            !streamer.readFloat(pitch) ||
            !streamer.readFloat(boostGain) ||
            !streamer.readFloat(boostFc) ||
            !streamer.readFloat(blend)) {
            return kResultFalse;
        }

        demon32[i].gain = gain;
        demon32[i].pitch = pitch;
        demon32[i].boostGain = boostGain;
        demon32[i].boostFc = boostFc;
        demon32[i].blend = blend;
        demon32[i].updateParams();

        demon64[i].gain = gain;
        demon64[i].pitch = pitch;
        demon64[i].boostGain = boostGain;
        demon64[i].boostFc = boostFc;
        demon64[i].blend = blend;
        demon64[i].updateParams();
    }

	return kResultOk;
}

tresult PLUGIN_API DemonProProcessor::getState (IBStream* state)
{
	// here we need to save the model
	IBStreamer streamer (state, kLittleEndian);

    /*
     * YOLO, we just write the state in the right order here
     * and hope it goes well.
     */
    streamer.writeInt32(bBypass ? 1 : 0);

	return kResultOk;
}

}
