#pragma once

#include <sstream>
#include <stdexcept>

#define eva_exception( e ) { std::stringstream ss; ss << __FILE__ << ":" << __LINE__ << " - " << e; throw std::runtime_error( ss.str() ); }
