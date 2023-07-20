![alt text](./img/SBLA.png)

```
a configuration file provides the necessary instructions for how to configure and operate a web server.

Like a blueprint, a configuration file defines the structure and behavior of the web server.
It specifies details such as which ports to listen on, which protocols to support,
how to handle requests, how to route traffic, and how to handle errors.
It also defines settings for various modules and plugins that provide additional 
functionality, such as caching, compression, and security.

Without a configuration file, a web server would not know how to operate 
and would not be able to serve content to clients. 
```


HTTP is a set of rules for communication = Protocol = a set of rules for communication between two computers. 

`how server deals with multipe clients
`
```
When a server needs to deal with multiple clients simultaneously, it typically uses a technique called concurrency or parallelism to handle them efficiently.

One common approach to concurrency is to use multiple threads or processes, each of which is responsible for handling a single client connection. 
When a new client connection arrives, the server creates a new thread or process to handle that connection and then returns to listening for new connections. 
This allows the server to handle multiple clients simultaneously, with each client connection running in its own thread or process.

Another approach is to use a single-threaded or event-driven architecture, 
where the server uses non-blocking I/O and event loops to handle multiple client connections without the need for multiple threads or processes.
```

h-to-n-s, or htons() (read: “Host to Network Short”).

---

Nginx lets you fine-tune your configuration down to three levels—at the protocol level (http block), the server level (server block), and the requested URI level (location block)

` Nginx HTTP Server (2018) pp 114`


```
http {
       server {
            listen 80;
            server_name example.com;
            access_log /var/log/nginx/example.com.log; 
            location ^~ /admin/ {
            index index.php; 
            }
    } 
}
```

The topmost directive block is the http block, in which you may declare a variety of configuration directives, as well as one or more server blocks.
A server block allows you to configure a virtual host, in other words, a website that is to be hosted on your machine. The server block, in this example,
contains some configuration that applies to all HTTP requests with a Host header exactly matching example.com.
Within this server block, you may insert one or more location blocks. These allow you to enable settings only when the requested URI matches the specified path.

` Nginx HTTP Server (2018) pp 50`

The HTTP core module is the component that contains all of the fundamental blocks, directives, and variables of the HTTP server.

It's enabled by default when you configure the build and Installing Nginx,
but it's actually optional—you can decide not to include it in your custom build. Doing so will completely disable all HTTP functionalities.

This module is the largest of all standard Nginx modules—it provides an impressive number of directives and variables. 

the HTTP module introduces three new logical blocks:

### http: 
This block is inserted at the root of the configuration file. It allows you to start defining directives and blocks from all modules related to the HTTP facet of Nginx. Although there is no real purpose in doing so, the block can be inserted multiple times, in which case the directive values inserted in the last block will override the previous ones.

### server: 
This block allows you to declare a website. In other words, a specific website (identified by one or more hostnames) becomes acknowledged by Nginx and receives its own configuration. This block can only be used within the http block.

### location: 
Lets you define a group of settings to be applied to a particular location on a website. This block can be used within a server block or nested within another location block.

` Nginx HTTP Server (2018) pp 75`

When Nginx receives an HTTP request, it matches the Host header of the request against all of the server blocks. The first server block to match this hostname is selected.
If no server block matches the desired host, Nginx selects the first server block that matches the parameters of the listen directive (for example, listen *:80 would be a catch-all for all requests received on port 80), giving priority to the first block that has the default_server option enabled on the listen directive.

` Nginx HTTP Server (2018) pp 78`


As a web server, NGINX's job is to serve static or dynamic contents to the clients. But how that content are going to be served is usually controlled by configuration files.

https://www.freecodecamp.org/news/the-nginx-handbook/#introduction-to-nginx-s-configuration-files
