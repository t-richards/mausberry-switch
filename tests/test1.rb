require_relative 'test_helper'

class Test1 < Minitest::Test
  def test_error_on_no_config_file
    system '../src/mausberry-switch'
    assert_equal(1, $?.exitstatus)
  end
end
