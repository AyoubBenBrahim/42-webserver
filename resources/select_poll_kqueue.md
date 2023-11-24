
intro : https://nima101.github.io/io_multiplexing
kqueue : https://nima101.github.io/kqueue_server

The time complexity (Big O notation) of each event handling mechanism can be described as follows:

1. `select`:
   - Time complexity: O(n), where n is the number of file descriptors being monitored.
   - The `select` mechanism performs a linear scan over all monitored file descriptors to determine the readiness of each descriptor. Therefore, the time required to process events grows linearly with the number of file descriptors.

2. `poll`:
   - Time complexity: O(n), where n is the number of file descriptors being monitored.
   - Similar to `select`, `poll` also performs a linear scan over all monitored file descriptors. The time required to process events scales linearly with the number of file descriptors.

3. `kqueue`:
   - Time complexity: O(1) or O(log n), depending on the specific use case and event handling scenario.
   - The `kqueue` mechanism utilizes an efficient event queue data structure, which allows for constant time (O(1)) or logarithmic time (O(log n)) event retrieval and processing, depending on the specific operations being performed.

It's important to note that these time complexities represent the event handling part of each mechanism and do not include any additional processing or overhead specific to your application logic. Additionally, the scalability and performance of each mechanism can vary depending on the underlying operating system and its implementation.