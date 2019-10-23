/*
Copyright 2019 DigitalOcean Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/**
 * @file prom_histogram_buckets.h
 * @brief https://prometheus.io/docs/concepts/metric_types/#histogram
 */

#ifndef PROM_HISTOGRAM_BUCKETS_INCLUDED
#define PROM_HISTOGRAM_BUCKETS_INCLUDED

typedef struct prom_histogram_buckets {
  int count;                  /**< Number of buckets */
  const double *upper_bounds; /**< The bucket values */
} prom_histogram_buckets_t;

prom_histogram_buckets_t* prom_histogram_buckets_new(size_t count, double bucket, ...);

/**
 * @brief the default histogram buckets: .005, .01, .025, .05, .1, .25, .5, 1, 2.5, 5, 10
 */
prom_histogram_buckets_t *prom_histogram_default_buckets;

/**
 * @brief Create linear buckets
 * @param start The first upper bound
 * @param width The distance between each upper bound in buckets
 * @param count The total number of buckets. The final +Inf bucket is not counted and not included in the returned
 * array
 * @return The array of upper bounds represented as double values
 *
 * The Array returned MUST be freed
 */
prom_histogram_buckets_t* prom_histogram_buckets_linear(double start, double width, size_t count);

/**
 * @brief Create linear buckets.
 * @param start The first upper bound. The value MUST be greater than 0.
 * @param factor The factor to apply to the previous upper bound to produce the next upper bound. The value MUST be
 * greater than 1.
 * @param count The total number of buckets. The final +Inf bucket is not counted and not included in the returned. The
 * value MUST be greater than or equal to 1
 * array
 * @return The array of upper bounds represented as double values
 *
 * The array returned MUST be freed
 */
prom_histogram_buckets_t* prom_histogram_buckets_exponential(double start, double factor, size_t count);


/**
 * @brief Destroy a prom_histogram_buckets_t
 */
int prom_histogram_buckets_destroy(prom_histogram_buckets_t *self);

/**
 * @brief Get the count of buckets
 */
size_t prom_histogram_buckets_count(prom_histogram_buckets_t *self);

#endif // PROM_HISTOGRAM_BUCKETS_INCLUDED
