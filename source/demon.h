#pragma once

#include "fx/fxobjects.h"
#include "paramids.h"

namespace MyVst {
using namespace Steinberg::Vst;

template <typename SampleType>
class Demon {
public:
    /*
     * Normalized parameters tuned directly by the user.
     */
    SampleType gain;
    SampleType pitch;
    SampleType boostGain;
    SampleType boostFc;
    SampleType blend;

    void updateParams(void);
    void reset(float sampleRate);
    void process(SampleType* buf, int nrSamples);
private:
    struct {
        SampleType gain;
    } cooked;

    PSMVocoder vocoder;
    AudioFilter boost;
};

template <typename SampleType>
void Demon<SampleType>::updateParams(void)
{
    cooked.gain = normdb2factor(gain, DEMON_GAIN_MIN, DEMON_GAIN_MAX);

    // Update vocoder params
    PSMVocoderParameters vocoder_params = vocoder.getParameters();
    vocoder_params.pitchShiftSemitones = PLAIN(pitch, DEMON_PITCH_MIN, DEMON_PITCH_MAX);
    vocoder.setParameters(vocoder_params);

    // Update boost params
    AudioFilterParameters params = boost.getParameters();
    params.fc = PLAIN(boostFc, DEMON_BOOST_FC_MIN, DEMON_BOOST_FC_MAX);
    params.boostCut_dB = PLAIN(boostGain, DEMON_BOOST_GAIN_MIN, DEMON_BOOST_GAIN_MAX);
    boost.setParameters(params);
}

template <typename SampleType>
void Demon<SampleType>::reset(float sampleRate)
{
    // Reset vocoder
    PSMVocoderParameters vocoder_params;
    vocoder_params.pitchShiftSemitones = PLAIN(pitch, DEMON_PITCH_MIN, DEMON_PITCH_MAX);
    vocoder_params.enablePeakPhaseLocking = true;
    vocoder_params.enablePeakTracking = true;
    vocoder.reset(sampleRate);
    vocoder.setParameters(vocoder_params);

    // Reset boost
    AudioFilterParameters boost_params;
    boost_params.algorithm = filterAlgorithm::kLowShelf;
    boost_params.fc = PLAIN(boostFc, DEMON_BOOST_FC_MIN, DEMON_BOOST_FC_MAX);
    boost_params.boostCut_dB = PLAIN(boostGain, DEMON_BOOST_GAIN_MIN, DEMON_BOOST_GAIN_MAX);
    boost.reset(sampleRate);
    boost.setParameters(boost_params);
}

template <typename SampleType>
void Demon<SampleType>::process(SampleType* buf, int nrSamples)
{
    while (nrSamples--) {
        SampleType xn, yn;

        xn = *buf * cooked.gain;
        yn = xn;
        yn = vocoder.processAudioSample(yn);
        yn = boost.processAudioSample(yn);
        *buf++ = yn * blend + xn * (1.0f - blend);
    }
}

}
