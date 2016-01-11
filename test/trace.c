#include <stdio.h>
#include <sys/time.h>

static FILE *fp_trace;

void
    __attribute__ ((constructor))
    trace_begin(void)
{
	fp_trace = fopen("trace.out", "w");
}

void
    __attribute__ ((destructor))
    trace_end(void)
{
	if (fp_trace != NULL) {
		fclose(fp_trace);
	}
}

void __cyg_profile_func_enter(void *func, void *caller)
{
	if (fp_trace != NULL) {
		struct timeval tv;
		unsigned long long ts;
		gettimeofday(&tv, 0);
		ts = (unsigned long long)tv.tv_sec * 1000000 + tv.tv_usec;
		fprintf(fp_trace, "e\t%p\t%p\t%llu\n", func, caller, ts);
	}
}

void __cyg_profile_func_exit(void *func, void *caller)
{
	if (fp_trace != NULL) {
		struct timeval tv;
		unsigned long long ts;
		gettimeofday(&tv, 0);
		ts = (unsigned long long)tv.tv_sec * 1000000 + tv.tv_usec;
		fprintf(fp_trace, "x\t%p\t%p\t%llu\n", func, caller, ts);
	}
}
