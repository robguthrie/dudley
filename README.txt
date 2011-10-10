Dudley
======

Overview
--------

Dudley is an unfinsihed program by me, Robert Guthre (rguthrie@gmail.com).

It is intended to become a file synchronization and backup system for your personal
photos, video and music collections. 

Dudley is part client and part tracker. The client is written in C++ with the Qt
Toolkit and the tracker is a Ruby On Rails application. This means the client can be 'native' on all platforms, something I think is necessary.

Dudley is designed to give cloud like features while not strictly requiring you
to purchase cloud storage, because it takes into account the redundancy already available 
in your local copies. 

Finally a feature I really want to have is the ability to seperate the collection definition from the storage, allowing dudley to recognise identical files between many collections, ultimately improving redundancy and availability of common files.

On a grand scale, I want to be able to build a decentralised file sharing social network.

How it is supposed to work
--------------------------

When you define a collection the Dudley client will index your files, this means fingerprinting (sha1 hashes) and recording
sizes, name, and modification time. When changes are made to the collection
state (IE: files are added, renamed, modified or deleted) dudley will log these changes 
via an automatic or manual polling process and append the change to the state log.

Currently the state log is stored in JSON text files in the root of your collection in a .dudley folder. I am interested in supporting an SQLite based log too.

The state log is synchronized via the Dudley server; a Rails app running at some URL.

As you add, remove, rename and modify files, dudley will detect the changes
and record them in additional JSON files in its .dudley folder. 

What works currrently
---------------------

In the Dudley C++ Client:
The filesystem scanner, which can scan a filesystem for changes and add to a JSON log.
The client side HTTP Server, which can serve files indexed by the client and JSON logs; it can also serve browsable index pages and accept Multipart form uploads.
A GUI that sits in the Taskbar, allowing you to see transfers, and manage collections.
UPNP support for the client.


The Ruby on Rails Server:
Not a lot here.. need to work on the sync between the clients and server.
The rails server should be able to tell which clients are online and redirect requests accordingly.

What needs to be done
---------------------

I want to turn these into specs:
Define logic for syncing logs at server.
Define access control.

Client checks in, tells the server the repos and their ids, that it holds, and it's client urls
Clients can define a priority list of callback urls, local ip's first to enable lan connections.
They also need to send a list of Repo names that they hold. Repo names will need to be unique to a server, and we need to include the version id
  EG: Callback url list:

  http://192.168.0.10:5635/
  http://202.20.98.72:5635/

  EG: Repo list
  robsmusic,234
  robspics,12

Requesting files from the network:
Either:
  Client requests file from the server, and the server replys with http redirects?
or
  Client requests file urls from the server, and the server replys with all the known active urls available. 

Rambling about understanding what I need to do follows:

The rails server should be able to tell what files each repo currently has. 
Dudley clients syncing the same collection need to resolve any issues. 
So each log file has an ordered id
A Dudley client keeps a list of uncommitted changes, it needs to get an id from the ruby server to commit, then log ids can just be numeric.
So to sync a client requests the last id, then it can determine which log files it needs to get to be synchronised.



