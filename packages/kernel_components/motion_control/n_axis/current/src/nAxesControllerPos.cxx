#include "kernel_components/nAxesControllerPos.hpp"
#include <assert.h>

namespace ORO_ControlKernel
{

  using namespace ORO_ControlKernel;


  nAxesPosController::nAxesPosController(unsigned int num_axes, 
					 std::string name)
    : nAxesPosController_typedef(name),
      _num_axes(num_axes), 
      _velocity_local(num_axes),
      _properties_read(false),
      _controller_gain("K", "Proportional Gain")
  {}


  nAxesPosController::~nAxesPosController(){};
  

  void nAxesPosController::pull()
  {
    // copy Input and Setpoint to local values
    _position_meas_DOI->Get(_position_meas_local);
    _position_desi_DOI->Get(_position_desi_local);
  }



  void nAxesPosController::calculate()
  {
    for(unsigned int i=0; i<_num_axes; i++)
     _velocity_local[i] = _controller_gain.value()[i] * (_position_desi_local[i] - _position_meas_local[i]);
  }

  
  void nAxesPosController::push()      
  {
    _velocity_DOI->Set(_velocity_local);
  }


  bool nAxesPosController::componentLoaded()
  {
    // get interface to Output data types
    if ( !nAxesPosController_typedef::Output::dObj()->Get("Velocity", _velocity_DOI) ){
      cerr << "nAxesPosController::componentLoaded() DataObjectInterface not found" << endl;
      return false;
    }
    return true;
  }


  bool nAxesPosController::componentStartup()
  {
    // check if updateProperties has been called
    assert(_properties_read);

    // get interface to Input/Setpoint data types
    if ( !nAxesPosController_typedef::Input::dObj(   )->Get("Position", _position_meas_DOI) ||
	 !nAxesPosController_typedef::SetPoint::dObj()->Get("Position", _position_desi_DOI) ){
      cerr << "nAxesPosController::componentStartup() DataObjectInterface not found" << endl;
      return false;
    }
    return true;
  }
  

  bool nAxesPosController::updateProperties(const PropertyBag& bag)
  {
    // properties have been read
    _properties_read = true;

    // get properties
    if (!composeProperty(bag, _controller_gain) ){
      cerr << "nAxesVelController::updateProperties() failed" << endl;
      return false;
    }

    // check size of properties
    assert(_controller_gain.value().size() >= _num_axes);

    return true;
  }

  

} // namespace
