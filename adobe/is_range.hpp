/*************************************************************************
*
* ADOBE CONFIDENTIAL
* ___________________
*
*  Copyright 2008 Adobe Systems Incorporated
*  All Rights Reserved.
*
* NOTICE:  All information contained herein is, and remains
* the property of Adobe Systems Incorporated and its suppliers,
* if any.  The intellectual and technical concepts contained
* herein are proprietary to Adobe Systems Incorporated and its
* suppliers and may be covered by U.S. and Foreign Patents,
* patents in process, and are protected by trade secret or copyright law.
* Dissemination of this information or reproduction of this material
* is strictly forbidden unless prior written permission is obtained
* from Adobe Systems Incorporated.
**************************************************************************/

#ifndef ADOBE_IS_RANGE_HPP
#define ADOBE_IS_RANGE_HPP

#include <adobe/type_inspection.hpp>

namespace adobe
{

	/*!
	\ingroup inspection

	\brief does T model the boost::range concepts?
	*/
	template <typename T>
	struct is_range
	{
		static const bool value = has_type_type<boost::range_iterator<T> >::value;
	};

}

#endif // include guard

