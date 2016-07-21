#include "dataSelectedEvent.hpp"

ofEvent<DataSelectedEventArgs> DataSelectedEvent::selected = ofEvent<DataSelectedEventArgs>();
ofEvent<DataSelectedEventArgs> DataSelectedEvent::deselected = ofEvent<DataSelectedEventArgs>();