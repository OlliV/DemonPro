#pragma once

#include "fx/fxobjects.h"
#include "paramids.h"

namespace MyVst {
using namespace Steinberg::Vst;

template <typename SampleType>
class Demon {
public:
    Demon(double defGain, double defPitch, double defBoostGain, double defBoostFc, double defBlend)
    {
        gain = defGain;
        pitch = defPitch;
        boostGain = defBoostGain;
        boostFc = defBoostFc;
        blend = defBlend;
    }

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
    void process(SampleType** inOut, int nrChannels, int nrSamples);
private:
    struct {
        SampleType gain;
        SampleType pitch;
    } cooked;

    AudioFilter boost;
};

template <typename SampleType>
void Demon<SampleType>::updateParams(void)
{
    cooked.gain = normdb2factor(drive, DEMON_GAIN_MIN, DEMON_GAIN_MAX);
    cooked.pitch = PLAIN(pitch, DEMON_PITCH_MIN, DEMON_PITCH_MAX);

    AudioFilterParameters params = boost.getParameters();
    params.fc = PLAIN(boostFc, DEMON_BOOST_FC_MIN, DEMON_BOOST_FC_MAX);
    params.boostCut_dB = PLAIN(boostGain, DEMON_BOOST_GAIN_MIN, DEMON_BOOST_GAIN_MAX);
    boost.setParameters(params);
}

template <typename SampleType>
void Demon<SampleType>::reset(float sampleRate)
{
    AudioFilterParameters params;
    params.algorithm = filterAlgorithm::kLowShelf;
    params.fc = PLAIN(boostFc, DEMON_BOOST_FC_MIN, DEMON_BOOST_FC_MAX);
    params.boostCut_dB = PLAIN(boostGain, DEMON_BOOST_GAIN_MIN, DEMON_BOOST_GAIN_MAX);
    boost.reset(sampleRate);
    boost.setParameters(params);
}

template <typename SampleType>
void Demon<SampleType>::process(SampleType* buf, int nrSamples)
{
    while (nrSamples--) {
        SampleType xn, yn;

        xn = *buf;
        yn = xn * cooked.gain;
        yn = pitchShift(yn);
        yn = boost.processAudioSample(yn);
        *buf++ = yn * blend + xn * (1.0f - blend);
    }
}

}
