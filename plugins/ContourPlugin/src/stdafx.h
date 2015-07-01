//
// std includes
//
#include <algorithm>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <exception>
#include <limits>

//
// c headers
//
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>

//
// boost includes
//
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/minmax_element.hpp>

#include <boost/log/common.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_multifile_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <boost/filesystem.hpp>

#include <boost/program_options.hpp>

#include <boost/format.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <boost/random.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>

#include <boost/numeric/conversion/bounds.hpp>
#include <boost/limits.hpp>

//
// platform specific for windows
//
#include <ExternalIncludes.h>
#include <memory/MemoryLeakDetection.h>