#ifndef OMP_UTILS_H
#define OMP_UTILS_H

#include <omp.h>

#define OMP_PARALLEL omp parallel default( shared )
#define OMP_PRIVATE private
#define OMP_ATOMIC omp atomic

#define OMP_CHECK_THREADS() gOmpThreads = omp_get_num_threads()

extern int gOmpThreads;

#define OMP_LOG() g_Log.WriteLog( "����(%s) ����OpenMP ����: ��%d���̹߳���", __FUNCTION__, gOmpThreads )


#endif