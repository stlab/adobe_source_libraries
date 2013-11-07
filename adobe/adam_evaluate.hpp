/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef ADOBE_ADAM_EVALUATE_HPP
#define ADOBE_ADAM_EVALUATE_HPP

#include <adobe/config.hpp>

#include <adobe/adam.hpp> // REVISIT (sparent) : need an adam_fwd.hpp
#include <adobe/adam_parser.hpp>
#include <adobe/external_model.hpp>

/*************************************************************************************************/

namespace adobe {

/*!
\defgroup property_model_evaluate Property Model Evaluate
\ingroup property_model
 */
/*************************************************************************************************/

/*!
\ingroup property_model_evaluate
 */
adam_callback_suite_t bind_to_sheet(sheet_t&);
adam_callback_suite_t bind_to_sheet(sheet_t&, external_model_t&);

/*************************************************************************************************/

} // namespace adobe

/*************************************************************************************************/

#endif // ADOBE_ADAM_EVALUATE_HPP

/*************************************************************************************************/
