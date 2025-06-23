/**************************************************************************************************/

/*

    ADOBE CONFIDENTIAL
    ___________________

    Copyright 2001 - 2004 Adobe Systems Incorporated
    All Rights Reserved.

    NOTICE: All information contained herein is, and remains the property of
    Adobe Systems Incorporated and its suppliers, if any. The intellectual and
    technical concepts contained herein are proprietary to Adobe Systems Incorporated
    and its suppliers and may be covered by U.S. and Foreign Patents, patents
    in process, and are protected by trade secret or copyright law. Dissemination
    of this information or reproduction of this material is strictly forbidden
    unless prior written permission is obtained from Adobe Systems Incorporated.

*/

/**************************************************************************************************/

// start_of_example

#include <adobe/memory.hpp>
#include <cassert>
#include <iostream>

int main() {
    // Basic usage with automatic cleanup
    {
        adobe::auto_resource<int*> ptr(new int(42));

        // Safe bool conversion allows checking if resource is valid
        if (ptr) {
            std::cout << "Value: " << *ptr.get() << std::endl; // prints "Value: 42"
        }

        // Resource is automatically deleted when ptr goes out of scope
    }

    // Transfer of ownership
    {
        adobe::auto_resource<int*> ptr1(new int(100));
        adobe::auto_resource<int*> ptr2(ptr1); // ptr1 transfers ownership to ptr2

        assert(!ptr1); // ptr1 is now empty
        assert(ptr2);  // ptr2 now owns the resource
        assert(*ptr2.get() == 100);
    }

    // Using reset to change the managed resource
    {
        adobe::auto_resource<int*> ptr(new int(1));
        ptr.reset(new int(2)); // Old resource deleted, new one managed
        assert(*ptr.get() == 2);

        ptr.reset(); // Release current resource, ptr becomes empty
        assert(!ptr);
    }

    // Using release to transfer ownership manually
    {
        adobe::auto_resource<int*> ptr(new int(99));
        int* raw_ptr = ptr.release(); // ptr no longer manages the resource

        assert(!ptr);
        assert(*raw_ptr == 99);

        delete raw_ptr; // Must manually delete since ownership was transferred
    }

    return 0;
}

// end_of_example
