#pragma once

#include <math.h>

#define LOG2DB 8.6858896380650365530225783783321f   // 20 / ln(10)
#define DB2LOG 0.11512925464970228420089957273422f // ln(10) / 20

#define GAIN_MIN               -20.0f
#define GAIN_MAX                20.0f
#define GAIN_DEFAULT_N          0.5f

#define NORM(v, min_v, max_v) \
    ((v) - (min_v)) / ((max_v) - (min_v))

#define PLAIN(v, min_v, max_v) \
    ((v) * ((max_v) - (min_v)) + (min_v))

#define DEMON_GAIN_MIN             -20.0f
#define DEMON_GAIN_MAX              20.0f
#define DEMON0_GAIN_DEFAULT_N       NORM(-5.0f, DEMON_GAIN_MIN, DEMON_GAIN_MAX)
#define DEMON1_GAIN_DEFAULT_N       NORM( 0.0f, DEMON_GAIN_MIN, DEMON_GAIN_MAX)
#define DEMON2_GAIN_DEFAULT_N       NORM( 5.0f, DEMON_GAIN_MIN, DEMON_GAIN_MAX)
#define DEMON_PITCH_MIN            -15.0f
#define DEMON_PITCH_MAX             15.0f
#define DEMON0_PITCH_DEFAULT_N      NORM(0.0f, DEMON_PITCH_MIN, DEMON_PITCH_MAX)
#define DEMON1_PITCH_DEFAULT_N      NORM(-6.20f, DEMON_PITCH_MIN, DEMON_PITCH_MAX)
#define DEMON2_PITCH_DEFAULT_N      NORM(-4.12f, DEMON_PITCH_MIN, DEMON_PITCH_MAX)
#define DEMON_BOOST_GAIN_MIN       -20.0f
#define DEMON_BOOST_GAIN_MAX        20.0f
#define DEMON0_BOOST_GAIN_DEFAULT_N NORM(0.0f, DEMON_BOOST_GAIN_MIN, DEMON_BOOST_GAIN_MAX)
#define DEMON1_BOOST_GAIN_DEFAULT_N NORM(7.8f, DEMON_BOOST_GAIN_MIN, DEMON_BOOST_GAIN_MAX)
#define DEMON2_BOOST_GAIN_DEFAULT_N NORM(0.0f, DEMON_BOOST_GAIN_MIN, DEMON_BOOST_GAIN_MAX)
#define DEMON_BOOST_FC_MIN          5.0f
#define DEMON_BOOST_FC_MAX          400.0f
#define DEMON0_BOOST_FC_DEFAULT_N   NORM(327.0f, DEMON_BOOST_FC_MIN, DEMON_BOOST_FC_MAX)
#define DEMON1_BOOST_FC_DEFAULT_N   NORM(327.0f, DEMON_BOOST_FC_MIN, DEMON_BOOST_FC_MAX)
#define DEMON2_BOOST_FC_DEFAULT_N   NORM(327.0f, DEMON_BOOST_FC_MIN, DEMON_BOOST_FC_MAX)
#define DEMON_BLEND_MIN             0.0f
#define DEMON_BLEND_MAX             1.0f
#define DEMON0_BLEND_DEFAULT_N      1.0f
#define DEMON1_BLEND_DEFAULT_N      1.0f
#define DEMON2_BLEND_DEFAULT_N      1.0f

static inline float db2norm(float db, float min, float max)
{
    return (db - min) / (max - min);
}

static inline float norm2db(float v, float min, float max)
{
    return (v * max) + (min * (1.0f - v));
}

static inline float normdb2factor(float v, float min, float max)
{
    return powf(10.0f, norm2db(v, min, max) / 20.0f);
}

enum {
    /** parameter ID */
    kBypassId = 0,      ///< Bypass value (we will handle the bypass process) (is automatable)
    kDemon0GainId,
    kDemon0PitchId,
    kDemon0BoostGainId,
    kDemon0BoostFcId,
    kDemon0BlendId,
    kDemon1GainId,
    kDemon1PitchId,
    kDemon1BoostGainId,
    kDemon1BoostFcId,
    kDemon1BlendId,
    kDemon2GainId,
    kDemon2PitchId,
    kDemon2BoostGainId,
    kDemon2BoostFcId,
    kDemon2BlendId,
    kNrParams
};
