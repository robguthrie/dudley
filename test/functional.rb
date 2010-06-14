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


#TRPL: page 357 - Kernel.open command can run programs with open('|./dudley') {|s| gets s}
require 'test/unit'

def cleanup
  # remove directories
  system 'rm -rf repo1'
end
cleanup

class TestDudley < Test::Unit::TestCase
  def test_track_operations
    dudleycmd = '/home/rob/src/dudley/dudley '
    Dir.mkdir 'repo1'
    Dir.chdir 'repo1' do
      result = open('|'+dudleycmd+' track init').read
      #assert empty new history
      assert result =~ /initialized history log/
      assert File.exists?('.dudley')
      
      #create a new file and test the response for correctness
      File.open('newfile', 'w') { |f| f.puts "blah blah blah, im a new file" }
      result = open('|'+dudleycmd+' track commit').read
      assert result =~ /add_file,newfile,(.{19}),30,fa44c5ccf6e7a157d484d3583bf8727803ff650f/

      #modify file and test the response for correctness
      File.open('newfile', 'w') { |f| f.puts "blah blah blah, im a modified file" }
      result = open('|'+dudleycmd+' track commit').read
      assert result =~ /modify_file,newfile,(.{19}),35,e05b16290898859ca71a2a31767a2415bb0af371/
      
      #rename file and test the response for correctness
      system('mv newfile goofile')
      result = open('|'+dudleycmd+' track commit').read
      assert result =~ /rename_file,newfile,goofile/
      
      #delete file and test the response for correctness
      system('rm goofile')
      result = open('|'+dudleycmd+' track commit').read
      assert result =~ /remove_file,goofile/
    end
    
    def test_add_file_is_synced
      #init repo a
      #touch a file in repo a
      #init repo b
      #compare states between repo a and repo b
      #show that repo b should request a file from repo a
      # define network friendly api for this..
      # file transfers are seperate tasks
      
      
    end 
    
    def test_sync_to_local
      # initialize a working repo
      # create a file, commit it
      # initialize a storage repo
      # sync these two repos
      # have saved identifiers like git with origin
    end
    
    def test_sync_to_http
      # client side sync..
    end
  end
  
#   def test_modified_file_is_detected
#     Dir.mkdir 'repo1'
#     Dir.chdir 'repo1' do
#       File.open('newfile', 'w') do |f|
#         f.puts "blah blah blah, im a new file"
#       end
#       res = system 'dudley init state1'
#       # assert that dudley found the file
#       assert res.contains
#       
#       system './dudley '
#       system 'dudley commit'
#     end
#     
#     Dir.create 'repo2'
#     Dir.chdir 'repo2' do
#       system 'dudley init state1'
#       system 'dudley update'
#       # and later.. push some stuff back
#     end
#   end
  
#   def test_sync_to_http
#     # what files to do want to sync?
#     # well dudley needs to check against the httpserver which files are missing
#     
#   end
end


