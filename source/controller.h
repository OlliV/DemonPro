//------------------------------------------------------------------------
// Copyright(c) 2021-2022 Olli Vanhoja.
//------------------------------------------------------------------------

#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"

namespace MyVst {
using namespace Steinberg;
using namespace Steinberg::Vst;

enum DemonProUnitId {
    DemonProRootUnitId = 0,
    DemonProDemon0UnitId,
    DemonProDemon1UnitId,
    DemonProDemon2UnitId,
    DemonProNrUnits
};

class DemonProController : public EditControllerEx1
{
public:
	DemonProController () = default;
	~DemonProController () SMTG_OVERRIDE = default;

    // Create function
	static FUnknown* createInstance (void* /*context*/)
	{
		return (IEditController*)new DemonProController;
	}

	// IPluginBase
	Steinberg::tresult PLUGIN_API initialize (Steinberg::FUnknown* context) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API terminate () SMTG_OVERRIDE;

	// EditController
	Steinberg::tresult PLUGIN_API setComponentState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::IPlugView* PLUGIN_API createView (Steinberg::FIDString name) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API setState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API setParamNormalized (ParamID tag,
                                                      ParamValue value) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getParamStringByValue (ParamID tag,
                                                         ParamValue valueNormalized,
                                                         String128 string) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getParamValueByString (ParamID tag,
                                                         TChar* string,
                                                         ParamValue& valueNormalized) SMTG_OVERRIDE;

 	//---Interface---------
	DEFINE_INTERFACES
        DEF_INTERFACE (IUnitInfo)
	END_DEFINE_INTERFACES (EditController)
    DELEGATE_REFCOUNT (EditController)
};

}
