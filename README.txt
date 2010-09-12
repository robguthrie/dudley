.dudley dir
    where we keep config
    instance.ini - settings to identify this instance
    repo.ini - global settings, shared amoung all the repos as they sync?

dudley http server
 index dirs and send files , nothing special for history logs

    grabbing files is a simple http get
    putting files same.. maybe s3 like?

mainwindows needs to hold
    repo list tab
        - list hosted repositories [name, path, num files, type]
        - each repo needs its own show/edit dialog too
    server tab - ip, port, registered dns as url, log window, transfers
    sync tab

    users tab - [user:password]

So need to create a list of transfers from a state - state comparison.

or should i think of it.. as what files are missing from the state. yes.

so blindly merge states so they compare as equal.
then each state should go about retrieving its missing files.
two very different things.

so today.. write tests for repo merge and make them pass

repos are hosted by dudley..they have a name then a path with some special path
prefix for queries and stuff

Ok so to get a list of files missing from the repo
bool hasFile(fileInfo) static
checking availability is different to checking for completeness

merging histories.. history logs should have sha1s as well as datetimes
need to write destructors to delete along my pointers

***********************
IMPLEMENT FileRepoState::difference and see how it goes!
************************

Second thinking of state to state comparison
---------------------------------------------
Different to updateState(oldState)
 - because there can be no conflicts?

OK so how do we do this?
When a state is updated - should we push it to the central server?
    probably
    centralised history? - private history?
    ok so two files with same filepath and different sha1, which file wins?
        ask user
        or default to newest if there is a big time difference?
        anyway its a conflict if both files have changed,...asdsadadadaf


we have a laptop pics repo, and a desktop pics repo

    empty repo on laptop
    user adds a file
    dudley notices new file in workingrepo, adds it to state, saves new commit log
    desktop pulls history from laptop to local cache of laptop history unmodified
    - always merge states?!?!? i think so

    maybe need to know last point in history both repos are the same


    no conflicts - just play history
        - the only time you can just play history, is when the current state of one repo matches some point of the other
        asks laptop for history logs which it saves to its local cache for the laptop
        assuming the repos have the same initial id you can just replay histories right?
        desktop can see laptop added a file
        takes the commit file verbatim
        both have the same commit history now - identical repos

    if
        OR merge repos
        builds a state object for the laptop repo
            we can then compare stateA(local-desktop) to stateB(remote-laptop)
                - is state A missing any files from state B
                - are any existing files in stateA different in stateB
                    files in stateA which are newer are left alone
                    files in stateA whcih are older than the modified versions in stateB are added to downloads
                - have any files been deleted?
        is this going to end up looking like the same commit that laptop made inthe first place?


    downloads the commit and saves it to the local history of local repo.

    desktop now does fileinventory and discovers it needs a file.
    - locally it should be able to see that the laptop repo should have the sha1 it needs
    - creates a list of possible sources for the file and attempts to download from each
    - file location resolution - some way to list which repos have the file

    then its just a matter of doing an http get for it.

    need to work out where to get the file from.

    in order to sync to an s3 bucket.. the pushing client must manage it's history
    on it's behalf

    needs to create a filerequest and push it to the webserver

Different to updateState(oldState)
compareState
- what files are missing?
    those which are not on disk
    those which have been modified in the


write a simple webserver in ruby
write a simple http server in dudley for sending files requested with sha1 and filename
include support for a history listing.
then make it a websocket server.. try to open a ws connection to the browser
http://upload.wikimedia.org/wikipedia/commons/c/c6/Http_request_telnet_ubuntu.png
request historys and save them
same with files. ensure they are the same when you get them back.
http://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol
http://doc.trolltech.com/qq/qq09-networkthread.html
http://doc.qt.nokia.com/4.6/network-programming.html

dudley dead simple file revision control and backup

   dudley allows you to record a history of changes to your files

   if we have an index of sha1 addressed objects, dudley can set filesystem state from the log


   commands
    init
        create a dudley working directory with the current working directory

    changes
        list local changes.

    accept
        write local changes to history.

    pull history from somewhere

    push history to somewhere
        show which files need to be inserted into the hashstore

    push files into collection

    pull files from collection

    sync
        pull history from master
        push history to master
        get any files required
        push any missing on remote

    ./dudley backup write
        insert files from status into hashstore.
        read the history, and go about inserting any files into the hashstore which need it.
        given some more work.. this represents the update of a dudley archive
        to backup to s3
            we need to calculate which files s3 has..
            and which we need to send.
