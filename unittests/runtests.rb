#!/usr/bin/ruby
require 'fileutils'

# first delete the dudley log from the test repo.
def reset_test_repo()
  test_repo_path = "../testrepo"
  test_repo_log_path = test_repo_path+"/.dudley"
  FileUtils.rm_rf(test_repo_log_path)
end

# now
reset_test_repo



