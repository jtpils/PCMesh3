namespace base2 {
	inline bool isPow2(unsigned i) {
	    return ((i - 1) & i) == 0;
	}

	inline uint8_t log2(unsigned n) {
	    assert(n > 0);

	    uint8_t log = 0;
	    while(true)
	    {
	        n >>= 1;
	        if (n == 0) {
	            break;
	        }
	        log++;
	    }
	    return log;
	}

	inline uint32_t ceil(int n) {
		uint8_t exp = log2(n);
		uint32_t ceiled = 1 << exp;
	    if(n > ceiled)
	        ceiled <<= 1;
	    return ceiled;
	}

	inline uint32_t floor(int n) {
		uint8_t exp = log2(n);
	    return 1 << exp;
	}
}