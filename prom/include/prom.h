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
 * @file prom.h
 * @brief Include prom.h to include the entire public API
 * @mainpage Welcome to the documentation site for prometheus-client-c!
 * @tableofcontents
 * @section Introduction
 *
 * prometheus-client-c is a small suite of Prometheus client libraries targeted for the C programming language.
 * In this brief tutorial you will learn how to create and register metrics, update metric samples, and expose metrics
 * over HTTP.
 *
 * @section Creating-and-Registering-Metrics Creating and Registering Metrics
 *
 * prometheus-client-c supports the following metric types:
 *
 * * [Counter](https://prometheus.io/docs/concepts/metric_types/#counter)
 * * [Gauge](https://prometheus.io/docs/concepts/metric_types/#gauge)
 * * [Histogram](https://prometheus.io/docs/concepts/metric_types/#histogram)
 *
 * To get started using one of the metric types, declare the metric at file scope. For example:
 *
 * @code{.c}
 *
 * #incldue "prom.h"
 *
 * prom_counter_t *my_counter;
 *
 * @endcode
 *
 * Next, create a metric initialization function. You can create the metric and register it with the default metric
 * collector registry in one chain of functions. A metric collector is responsible for collecting metrics and returning
 * them. A metric collector registry is declared in global scope and contains metric collectors. More on this later...
 *
 * To create a metric and register it with the default metric collector registry in one shot, you may chain the metric
 * constructor into the prom_collector_registry_must_register_metric function. For example:
 *
 * @code{.c}
 *
 * void foo_metric_init(void) {
 *   my_counter = prom_collector_registry_must_register_metric(prom_counter_new("my_counter", "counts things", 0, NULL));
 * }
 *
 * @endcode
 *
 * The first argument to prom_counter_new is the counter name. The second argument is the counter description. The third
 * argument is the number of metric labels. In this case, we will only have one metric sample for this metric so we pass
 * 0 to specify that no labels will be used. The 4th argument is an array of strings storing the metric labels. Since we
 * have none, we pass NULL. A call to foo_metric_init within the program's main function will initialize the metrics
 * for the file we just created to the default prometheus metric collector registery called
 * PROM_COLLECTOR_REGISTRY_DEFAULT
 *
 * @section Updating-Metric-Sample-Values Updating Metric Sample Values
 *
 * Now that we have a metric configured for creation and registration, we can update our metric within any of the
 * functions of the file in which it was declared. For example:
 *
 * @code{.c}
 *
 * void my_lib_do_something(void) {
 *   printf("I did a really important thing!\n");
 *   prom_counter_inc(my_counter, NULL);
 * }
 * @endcode
 *
 * This function will increment the default metric sample for my_counter. Since we are not using metric labels, we pass
 * NULL as the second argument.
 *
 * @section Program-Initialization Program Initialization
 *
 * At the start of the program's main function you need to do two things:
 *
 * * Initialize the default metric collector registry:
 *
 * @code{.c}
 *
 * prom_collector_registry_default_init();
 *
 * @endcode
 *
 * * For each file containing prometheus metrics, call its corresponding metric initialization function
 *
 * @code{.c}
 *
 * foo_metric_init()
 *
 * @endcode
 *
 * After initialization is complete, you may proceed to do work and update your metrics.
 *
 * @section Metric-Exposition-Over-HTTP Metric Exposition Over HTTP
 *
 * @todo Describe how to use libpromhttp to expose metrics over HTTP
 *
 * @section Where-To-Go-From-Here Where to Go From Here?
 *
 * Take a look at the [Files](https://github.internal.digitalocean.com/pages/timeseries/prometheus-client-c/files.html)
 * tab in this documentation site for more information about the public API available to you. Also, you can take a look
 * at the examples directory at the
 * [Github repository](https://github.internal.digitalocean.com/timeseries/prometheus-client-c) for inspiration.
 */

#ifndef PROM_INCLUDED
#define PROM_INCLUDED

#include "prom_alloc.h"
#include "prom_collector.h"
#include "prom_collector_registry.h"
#include "prom_counter.h"
#include "prom_gauge.h"
#include "prom_histogram.h"
#include "prom_histogram_buckets.h"
#include "prom_linked_list.h"
#include "prom_map.h"
#include "prom_metric.h"
#include "prom_metric_sample.h"
#include "prom_metric_sample_histogram.h"

#endif //  PROM_INCLUDED