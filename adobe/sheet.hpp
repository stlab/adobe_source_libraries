/*
    Copyright 2013 Adobe
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/
/**************************************************************************************************/

#ifndef ADOBE_SHEET_HPP
#define ADOBE_SHEET_HPP

/**************************************************************************************************/

#include <adobe/config.hpp>

/**************************************************************************************************/

namespace adobe {

/**************************************************************************************************/

#ifdef ADOBE_CONCEPTS

/**************************************************************************************************/

auto concept BasicSheetConcept<typename T>{};

/**************************************************************************************************/

auto concept SheetConcept<typename T> : BasicSheetConcept<T> {
    void touch(T & t, const name_t*, const name_t*);
};

/**************************************************************************************************/

auto concept SheetMFConcept<typename T> : BasicSheetConcept<T> {
    void T::touch(const name_t*, const name_t*);
};

/**************************************************************************************************/

template <SheetMFConcept T>
concept_map SheetConcept<T>{
    inline void touch(T & t, const name_t* x, const name_t* y){t.touch(x, y);
}
}
;

/**************************************************************************************************/

template <BasicSheetConcept T>
concept_map BasicSheetConcept<std::reference_wrapper<T>>{};

/**************************************************************************************************/

template <SheetConcept T>
concept_map SheetConcept<std::reference_wrapper<T>>{
    inline void touch(std::reference_wrapper<T> & r, const name_t* x,
                      const name_t* y){SheetConcept<T>::touch(static_cast<T&>(r), x, y);
}
}
;

/**************************************************************************************************/

#endif

/**************************************************************************************************/
}

/**************************************************************************************************/

#endif
