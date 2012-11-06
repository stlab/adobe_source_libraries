/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

/*************************************************************************************************/


/*
    REVISIT (sparent) : As this file is expanded it should include boost/config.hpp and use the
    macros defined there to perform these checks. For now this, header is being maintained
    without the additional dependencies. 
*/


#ifdef __MWERKS__

    #if __MWERKS__ < 0x3003
        #error "Metrowerks Version 8.3 or newer is required."
    #endif

    #if __MWERKS__ == 0x3003
        #include <string>
        
        /*
        NOTE (sparent) : The leading double underscore should not have been used (reserved by
        the standard for compiler vendors). However, this fix is confined to a single version
        of a single compiler and Photoshop already defined the symbol so I'm letting it go.
        */

        #ifndef __ADOBE_FIX_FOR_BASIC_STRING_ALLOCATION__
            #warning "File <string> may not contain fix for basic string allocation."
                /*
                    NOTE (sparent) : Line 1994 of <string> must be changed from:
                        traits::assign(*(data_ + n), charT());
                    to:
                        traits::assign(*(data_ + size_), charT());
                        
                    After making the change the following line should be added to the file:
                    
                    #define __ADOBE_FIX_FOR_BASIC_STRING_ALLOCATION__
                */
        #endif

    #endif

#endif
