#!/usr/bin/ruby

# test script
#     create a dir repo1
#     initialize it with dudley as state1
#     copy in some files
#     commit
#     create another dir repo2
#     initialize it to state1
#     update
#     ensure that files came across
# 
#     delete from repo2
#     rename something in repo2
#     modifiy something in repo2
#     add something new to repo2
#     commit
#     update repo1
# 
#     ensure that the dirs have the same files

def setup
  Dir.create 'repo1'
  Dir.chdir 'repo1' do
    File.open('newfile', 'w') do |f|
      f.puts "blah blah blah, im a new file"
    end
    system 'dudley init state1'
    system 'dudley commit'
  end
  
  Dir.create 'repo2'
  Dir.chdir 'repo2' do
    system 'dudley init state1'
    system 'dudley update'
    # and later.. push some stuff back
  end
end

def cleanup
  # remove directories
end

setup
cleanup