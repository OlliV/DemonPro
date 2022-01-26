//------------------------------------------------------------------------
// Copyright(c) 2021-2022 Olli Vanhoja.
//------------------------------------------------------------------------

#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/base/ustring.h"
#include "pluginterfaces/vst/ivstmidicontrollers.h"
#include "base/source/fstreamer.h"
#include "controller.h"
#include "cids.h"
#include "paramids.h"
#include "mdaParameter.h"
#include "vstgui/plugin-bindings/vst3editor.h"

using namespace Steinberg;
using namespace Steinberg::Vst;

namespace MyVst {

class GainParameter : public Parameter
{
public:
    GainParameter (const char *name, int32 flags, int32 id, float min, float max);

    void toString (ParamValue normValue, String128 string) const SMTG_OVERRIDE;
    bool fromString (const TChar* string, ParamValue& normValue) const SMTG_OVERRIDE;

private:
    float gain_min;
    float gain_max;
};

GainParameter::GainParameter (const char *name, int32 flags, int32 id, float min_db, float max_db)
{
    Steinberg::UString (info.title, USTRINGSIZE (info.title)).assign (USTRING (name));
    Steinberg::UString (info.units, USTRINGSIZE (info.units)).assign (USTRING ("dB"));

    gain_min = min_db;
    gain_max = max_db;

    info.flags = flags;
    info.id = id;
    info.stepCount = 0;
    info.defaultNormalizedValue = db2norm(0.0f, min_db, max_db);
    info.unitId = kRootUnitId;

    setNormalized(info.defaultNormalizedValue);
}

void GainParameter::toString (ParamValue normValue, String128 string) const
{
    char text[32];
    float db = norm2db(normValue, gain_min, gain_max);

    snprintf(text, sizeof(text), "%.2f", db);
    text[sizeof(text) - 1] = '\0';

    Steinberg::UString (string, 128).fromAscii(text);
}

bool GainParameter::fromString (const TChar* string, ParamValue& normValue) const
{
    String wrapper ((TChar*)string); // don't know buffer size here!
    double tmp = 0.0;

    if (wrapper.scanFloat(tmp)) {
        normValue = db2norm((float)tmp, gain_min, gain_max);

        return true;
    }
    return false;
}

tresult PLUGIN_API DemonProController::initialize (FUnknown* context)
{
	//---do not forget to call parent ------
	tresult result = EditControllerEx1::initialize(context);
	if (result != kResultOk) {
		return result;
	}

    //--- Create Units-------------
    UnitInfo unitInfo;
    Unit* unit;
    Parameter *param;

    // Demon 1
    unitInfo.id = DemonProDemon0UnitId;
    unitInfo.parentUnitId = kRootUnitId; // attached to the root unit
    Steinberg::UString(unitInfo.name, USTRINGSIZE(unitInfo.name)).assign(USTRING("Demon 1"));
    unitInfo.programListId = kNoProgramListId;
    addUnit(new Unit(unitInfo));

    // Demon 2
    unitInfo.id = DemonProDemon1UnitId;
    unitInfo.parentUnitId = kRootUnitId; // attached to the root unit
    Steinberg::UString(unitInfo.name, USTRINGSIZE(unitInfo.name)).assign(USTRING("Demon 2"));
    unitInfo.programListId = kNoProgramListId;
    addUnit(new Unit(unitInfo));

    // Demon 3
    unitInfo.id = DemonProDemon2UnitId;
    unitInfo.parentUnitId = kRootUnitId; // attached to the root unit
    Steinberg::UString(unitInfo.name, USTRINGSIZE(unitInfo.name)).assign(USTRING("Demon 3"));
    unitInfo.programListId = kNoProgramListId;
    addUnit(new Unit(unitInfo));

    const int32 stepCountToggle = 1;
    ParamValue defaultVal = 0;
    int32 flags = ParameterInfo::kCanAutomate | ParameterInfo::kIsBypass;
    param = parameters.addParameter(STR16("Bypass"), nullptr, stepCountToggle, defaultVal, flags, kBypassId);
    param->setUnitID(DemonProRootUnitId);

    // Demon 1: Gain
    param = new GainParameter("Gain", ParameterInfo::kCanAutomate, kDemon0GainId, GAIN_MIN, GAIN_MAX);
    param->setNormalized(DEMON0_GAIN_DEFAULT_N);
    param->setPrecision(2);
    param->setUnitID(DemonProDemon0UnitId);
    parameters.addParameter(param);

    // Demon 1: Pitch
    param = new Steinberg::Vst::mda::ScaledParameter(USTRING("Pitch"), USTRING("semitones"), 0, DEMON0_PITCH_DEFAULT_N, ParameterInfo::kCanAutomate, kDemon0PitchId, DEMON_PITCH_MIN, DEMON_PITCH_MAX);
    param->setNormalized(DEMON0_PITCH_DEFAULT_N);
    param->setPrecision(2);
    param->setUnitID(DemonProDemon0UnitId);
    parameters.addParameter(param);

    // Demon 1: Boost Gain
    param = new GainParameter("Boost Gain", ParameterInfo::kCanAutomate, kDemon0BoostGainId, GAIN_MIN, GAIN_MAX);
    param->setNormalized(DEMON0_BOOST_GAIN_DEFAULT_N);
    param->setPrecision(2);
    param->setUnitID(DemonProDemon0UnitId);
    parameters.addParameter(param);

    // Demon 1: Boost fc
    param = new Steinberg::Vst::mda::ScaledParameter(USTRING("Boost Freq"), USTRING("Hz"), 0, DEMON0_BOOST_FC_DEFAULT_N, ParameterInfo::kCanAutomate, kDemon0BoostFcId, DEMON_BOOST_FC_MIN, DEMON_BOOST_FC_MAX);
    param->setNormalized(DEMON0_BOOST_FC_DEFAULT_N);
    param->setPrecision(0);
    param->setUnitID(DemonProDemon0UnitId);
    parameters.addParameter(param);

    // Demon 1: Blend
    param = new Steinberg::Vst::mda::ScaledParameter(USTRING("Blend"), USTRING(":1"), 0.0f, 1.0f, ParameterInfo::kCanAutomate, kDemon0BlendId);
    param->setNormalized(DEMON0_BLEND_DEFAULT_N);
    param->setPrecision(2);
    param->setUnitID(DemonProDemon0UnitId);
    parameters.addParameter(param);

    // Demon 2: Gain
    param = new GainParameter("Gain", ParameterInfo::kCanAutomate, kDemon1GainId, GAIN_MIN, GAIN_MAX);
    param->setNormalized(DEMON1_GAIN_DEFAULT_N);
    param->setPrecision(2);
    param->setUnitID(DemonProDemon1UnitId);
    parameters.addParameter(param);

    // Demon 2: Pitch
    param = new Steinberg::Vst::mda::ScaledParameter(USTRING("Pitch"), USTRING("semitones"), 0, DEMON1_PITCH_DEFAULT_N, ParameterInfo::kCanAutomate, kDemon1PitchId, DEMON_PITCH_MIN, DEMON_PITCH_MAX);
    param->setNormalized(DEMON1_PITCH_DEFAULT_N);
    param->setPrecision(2);
    param->setUnitID(DemonProDemon1UnitId);
    parameters.addParameter(param);

    // Demon 2: Boost Gain
    param = new GainParameter("Boost Gain", ParameterInfo::kCanAutomate, kDemon1BoostGainId, GAIN_MIN, GAIN_MAX);
    param->setNormalized(DEMON1_BOOST_GAIN_DEFAULT_N);
    param->setPrecision(2);
    param->setUnitID(DemonProDemon1UnitId);
    parameters.addParameter(param);

    // Demon 2: Boost fc
    param = new Steinberg::Vst::mda::ScaledParameter(USTRING("Boost Freq"), USTRING("Hz"), 0, DEMON1_BOOST_FC_DEFAULT_N, ParameterInfo::kCanAutomate, kDemon1BoostFcId, DEMON_BOOST_FC_MIN, DEMON_BOOST_FC_MAX);
    param->setNormalized(DEMON1_BOOST_FC_DEFAULT_N);
    param->setPrecision(0);
    param->setUnitID(DemonProDemon1UnitId);
    parameters.addParameter(param);

    // Demon 2: Blend
    param = new Steinberg::Vst::mda::ScaledParameter(USTRING("Blend"), USTRING(":1"), 0.0f, 1.0f, ParameterInfo::kCanAutomate, kDemon1BlendId);
    param->setNormalized(DEMON1_BLEND_DEFAULT_N);
    param->setPrecision(2);
    param->setUnitID(DemonProDemon1UnitId);
    parameters.addParameter(param);

    // Demon 3: Gain
    param = new GainParameter("Gain", ParameterInfo::kCanAutomate, kDemon2GainId, GAIN_MIN, GAIN_MAX);
    param->setNormalized(DEMON2_GAIN_DEFAULT_N);
    param->setPrecision(2);
    param->setUnitID(DemonProDemon2UnitId);
    parameters.addParameter(param);

    // Demon 3: Pitch
    param = new Steinberg::Vst::mda::ScaledParameter(USTRING("Pitch"), USTRING("semitones"), 0, DEMON2_PITCH_DEFAULT_N, ParameterInfo::kCanAutomate, kDemon2PitchId, DEMON_PITCH_MIN, DEMON_PITCH_MAX);
    param->setNormalized(DEMON2_PITCH_DEFAULT_N);
    param->setPrecision(2);
    param->setUnitID(DemonProDemon2UnitId);
    parameters.addParameter(param);

    // Demon 3: Boost Gain
    param = new GainParameter("Boost Gain", ParameterInfo::kCanAutomate, kDemon2BoostGainId, GAIN_MIN, GAIN_MAX);
    param->setNormalized(DEMON2_BOOST_GAIN_DEFAULT_N);
    param->setPrecision(2);
    param->setUnitID(DemonProDemon2UnitId);
    parameters.addParameter(param);

    // Demon 3: Boost fc
    param = new Steinberg::Vst::mda::ScaledParameter(USTRING("Boost Freq"), USTRING("Hz"), 0, DEMON1_BOOST_FC_DEFAULT_N, ParameterInfo::kCanAutomate, kDemon2BoostFcId, DEMON_BOOST_FC_MIN, DEMON_BOOST_FC_MAX);
    param->setNormalized(DEMON2_BOOST_FC_DEFAULT_N);
    param->setPrecision(0);
    param->setUnitID(DemonProDemon2UnitId);
    parameters.addParameter(param);

    // Demon 3: Blend
    param = new Steinberg::Vst::mda::ScaledParameter(USTRING("Blend"), USTRING(":1"), 0.0f, 1.0f, ParameterInfo::kCanAutomate, kDemon2BlendId);
    param->setNormalized(DEMON2_BLEND_DEFAULT_N);
    param->setPrecision(2);
    param->setUnitID(DemonProDemon2UnitId);
    parameters.addParameter(param);

	return result;
}

tresult PLUGIN_API DemonProController::terminate ()
{
	// Here the Plug-in will be de-instanciated, last possibility to remove some memory!

	//---do not forget to call parent ------
	return EditControllerEx1::terminate();
}

tresult PLUGIN_API DemonProController::setComponentState (IBStream* state)
{
    if (!state)
        return kResultFalse;

    IBStreamer streamer (state, kLittleEndian);
    int32 savedBypass;

    if (!streamer.readInt32(savedBypass)) {
        return kResultFalse;
    }

    setParamNormalized(kBypassId, savedBypass ? 1 : 0);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            float v;

            if (!streamer.readFloat(v)) {
                return kResultFalse;
            }

            setParamNormalized(kDemon0GainId + i * 5 + j, v);
        }
    }

	return kResultOk;
}

tresult PLUGIN_API DemonProController::setState (IBStream* state)
{
	// Here you get the state of the controller

	return kResultTrue;
}

tresult PLUGIN_API DemonProController::getState (IBStream* state)
{
	// Here you are asked to deliver the state of the controller (if needed)
	// Note: the real state of your plug-in is saved in the processor

	return kResultTrue;
}

IPlugView* PLUGIN_API DemonProController::createView (FIDString name)
{
	// Here the Host wants to open your editor (if you have one)
	if (FIDStringsEqual (name, Vst::ViewType::kEditor))
	{
		// create your editor here and return a IPlugView ptr of it
		auto* view = new VSTGUI::VST3Editor (this, "view", "editor.uidesc");
		return view;
	}
	return nullptr;
}

tresult PLUGIN_API DemonProController::setParamNormalized (Vst::ParamID tag, Vst::ParamValue value)
{
	// called by host to update your parameters
	tresult result = EditControllerEx1::setParamNormalized (tag, value);
	return result;
}

tresult PLUGIN_API DemonProController::getParamStringByValue (Vst::ParamID tag, Vst::ParamValue valueNormalized, Vst::String128 string)
{
	// called by host to get a string for given normalized value of a specific parameter
	// (without having to set the value!)
	return EditControllerEx1::getParamStringByValue (tag, valueNormalized, string);
}

tresult PLUGIN_API DemonProController::getParamValueByString (Vst::ParamID tag, Vst::TChar* string, Vst::ParamValue& valueNormalized)
{
	// called by host to get a normalized value from a string representation of a specific parameter
	// (without having to set the value!)
	return EditControllerEx1::getParamValueByString (tag, string, valueNormalized);
}

}
