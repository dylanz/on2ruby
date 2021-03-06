require 'test_helper'

class FlixEngineTest < Test::Unit::TestCase
  def setup
    @flix_engine = On2::FlixEngine.new('localhost', 0)
  end
  
  def test_bad_rpchost
    assert_raises(On2::FlixEngineError) {
      On2::FlixEngine.new('asdf', 0)
    }
  end
  
  def test_version
    assert @flix_engine.version
  end
  
  def test_copyright
    assert @flix_engine.copyright
  end
  
  def test_source_file
    @flix_engine.source_path = TEST_INPUT_MOVIE_PATH
    assert_equal TEST_INPUT_MOVIE_PATH, @flix_engine.source_path
  end
  
  def test_output_file
    @flix_engine.output_path = TEST_OUTPUT_MOVIE_PATH
    assert_equal TEST_OUTPUT_MOVIE_PATH, @flix_engine.output_path
  end
  
  def test_encode
    @flix_engine.source_path = TEST_INPUT_MOVIE_PATH
    @flix_engine.output_path = TEST_OUTPUT_MOVIE_PATH
    @flix_engine.encode
    assert @flix_engine.encoding?
    while (@flix_engine.encoding?) do
      sleep 3
      assert @flix_engine.percent_complete
    end
    assert !@flix_engine.encoding?
    assert 100, @flix_engine.percent_complete
    assert File.exists?(TEST_OUTPUT_MOVIE_PATH)
  end
  
  def teardown
    # Force cleanup of the flix engine
    # Not really needed but doesn't hurt
    @flix_engine.finalize
    File.delete(TEST_OUTPUT_MOVIE_PATH) if File.exists?(TEST_OUTPUT_MOVIE_PATH)
  end
end
