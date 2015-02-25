require "zlib"

class Plane
  def initialize(viewport = nil)
    @bitmap = nil
    @viewport = viewport
    @visible = true
    @z = 0
    @ox = 0
    @oy = 0
    @zoom_x = 1.0
    @zoom_y = 1.0
    @opacity = 255
    @blend_type = 0
    @color = Color.new
    @tone = Tone.new

    @disposed = false
  end
  def dispose; @disposed = true; nil; end
  def disposed?; @disposed; end
  attr_accessor :bitmap, :viewport, :visible, :z, :ox, :oy, :zoom_x
  attr_accessor :zoom_y, :opacity, :blend_type, :color, :tone
  def color=(color); @color.set(color); end
  def tone=(tone); @tone.set(tone); end
end
class Table
  def initialize(*args)
    if 1 <= args.length && args.length <= 3 then
      @dim = args.length
      xsize, ysize, zsize = *args, 1, 1
      @xsize = xsize
      @ysize = ysize
      @zsize = zsize
    else
      raise ArgumentError.new("wrong number of arguments (#{args.length} for 1..2)")
    end
  end
  def resize(*args)
    if 1 <= args.length && args.length <= 3 then
      @dim = args.length
      xsize, ysize, zsize = *args, 1, 1
      @xsize = xsize
      @ysize = ysize
      @zsize = zsize
    else
      raise ArgumentError.new("wrong number of arguments (#{args.length} for 1..2)")
    end
  end
  def self._load(s)
    Table.new(0)
  end
end
class Tilemap
  def initialize(viewport = nil)
    @bitmaps = BitmapArray.new
    @map_data = nil
    @flash_data = nil
    @flags = nil
    @viewport = viewport
    @visible = true
    @ox = 0
    @oy = 0

    @disposed = false
  end
  def dispose; @disposed = true; nil; end
  def disposed?; @disposed; end
  attr_reader :bitmaps
  attr_accessor :map_data, :flash_data, :flags, :viewport, :visible, :ox, :oy
  class BitmapArray
    def initialize
      @val = Array.new(9, nil)
    end
    def [](i); @val[i]; end
    def []=(i,x); @val[i] = x; end
  end
end
class RGSSError < StandardError; end
class RGSSReset < Exception; end

module Audio
  def self.setup_midi; end
  def self.bgm_play(filename, volume = 100, pitch = 100, pos = 0); end
  def self.bgm_stop; end
  def self.bgm_fade(time); end
  def self.bgm_pos; 0; end
  def self.bgs_play(filename, volume = 80, pitch = 100, pos = 0); end
  def self.bgs_stop; end
  def self.bgs_fade(time); end
  def self.bgs_pos; 0; end

  def self.me_play(filename, volume = 100, pitch = 100); end
  def self.me_stop; end
  def self.me_fade(time); end
  def self.se_play(filename, volume = 80, pitch = 100); end
  def self.se_stop; end
  def self.se_fade(time); end
end
module Graphics
  def self.wait(duration)
    duration.times { Graphics.update }
  end
  def self.fadeout(duration); Graphics.wait(duration); end
  def self.fadein(duration); Graphics.wait(duration); end
  def self.transition(duration = 10, filename = nil, vague = 40)
    Graphics.wait(duration)
  end
  def self.snap_to_bitmap; Bitmap.new(width, height); end
  def self.frame_reset; end
  def self.width; @width; end
  def self.height; @height; end
  def self.resize_screen(width, height)
    @width = width
    @height = height
  end
  def self.play_movie(filename); end
  self.resize_screen(544, 416)
  class << self
    attr_accessor :frame_rate, :frame_count, :brightness
  end
end
module Input
  def self.update; end
  def self.press?(sym); false; end
  def self.trigger?(sym); false; end
  def self.repeat?(sym); false; end
  def self.dir4; 0; end
  def self.dir8; 0; end
  DOWN = :DOWN
  LEFT = :LEFT
  RIGHT = :RIGHT
  UP = :UP
  A = :A
  B = :B
  C = :C
  X = :X
  Y = :Y
  Z = :Z
  L = :L
  R = :R
  SHIFT = :SHIFT
  CTRL = :CTRL
  ALT = :ALT
  F5 = :F5
  F6 = :F6
  F7 = :F7
  F8 = :F8
  F9 = :F9
end

def rgss_main
  yield
rescue RGSSReset
  retry
end

load_data("Data/Scripts.rvdata2").each do|num,title,script|
  s = Zlib::Inflate::inflate(script)
  s.force_encoding("utf-8")
  eval(s, binding, title)
end
