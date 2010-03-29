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
