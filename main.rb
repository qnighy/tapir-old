load "rpg.rb"
Dir.chdir("/home/qnighy/workdir/vxace/Sakusaku")
require "zlib"

def saturate(num, minval, maxval)
  if num <= minval then
    minval
  elsif num >= maxval then
    maxval
  else
    num
  end
end

class Bitmap
  def initialize(*args)
    if args.length == 1 then
      filename, = *args
      @width = 1
      @height = 1
    elsif args.length == 2 then
      width, height = *args
      @width = width
      @height = height
    else
      raise ArgumentError.new("wrong number of arguments (#{args.length} for 1..2)")
    end
    @disposed = false
    @font = Font.new
    nil
  end
  def dispose; @disposed = true; nil; end
  def disposed?; @disposed; end
  def width; @width; end
  def height; @height; end
  def rect; Rect.new(0, 0, @width, @height); end
  def blt(x, y, src_bitmap, src_rect, opacity = 255); end
  def stretch_blt(dest_rect, src_bitmap, src_rect, opacity = 255); end
  def fill_rect(*args)
    if args.length == 5 then
      x, y, width, height, color = *args
    elsif args.length == 2 then
      rect, color = *args
    else
      raise ArgumentError.new("wrong number of arguments (#{args.length} for 2 or 5)")
    end
  end
  def gradient_fill_rect(*args)
    if args.length == 7 then
      x, y, width, height, color1, color2, vertical = *args
    elsif args.length == 6 then
      x, y, width, height, color1, color2, vertical = *args, false
    elsif args.length == 4 then
      rect, color1, color2, vertical = *args
    elsif args.length == 3 then
      rect, color1, color2, vertical = *args, false
    else
      raise ArgumentError.new("wrong number of arguments (#{args.length} for 3..4 or 6..7)")
    end
  end
  def clear; end
  def clear_rect(*args)
    if args.length == 4 then
      x, y, width, height = *args
    elsif args.length == 1 then
      rect, = *args
    else
      raise ArgumentError.new("wrong number of arguments (#{args.length} for 1 or 4)")
    end
  end
  def get_pixel(x, y); Color.new; end
  def set_pixel(x, y, color); nil; end
  def hue_change(hue); nil; end
  def blur; nil; end
  def radial_blur(angle, division); nil; end
  def draw_text(*args)
    if args.length == 6 then
      x, y, width, height, str, align = *args
    elsif args.length == 5 then
      x, y, width, height, str, align = *args, 0
    elsif args.length == 3 then
      rect, str, align = *args
    elsif args.length == 2 then
      rect, str, align = *args, 0
    else
      raise ArgumentError.new("wrong number of arguments (#{args.length} for 2..3 or 5..6)")
    end
  end
  def text_size(str); Rect.new; end
  attr_accessor :font
  def font=(font); @font.set(font); end
end
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
class Sprite
  def initialize(viewport = nil)
    @bitmap = nil
    @src_rect = Rect.new
    @viewport = viewport
    @visible = true
    @x = 0
    @y = 0
    @z = 0
    @ox = 0
    @oy = 0
    @zoom_x = 1.0
    @zoom_y = 1.0
    @angle = 0
    @wave_amp = 0
    @wave_length = 180
    @wave_speed = 360
    @wave_phase = 0.0
    @mirror = false
    @bush_depth = 0
    @bush_opacity = 128
    @opacity = 255
    @blend_type = 0
    @color = Color.new
    @tone = Tone.new

    @disposed = false
  end
  def dispose; @disposed = true; nil; end
  def disposed?; @disposed; end
  def update; end
  def width; @src_rect.width; end
  def height; @src_rect.height; end
  attr_accessor :bitmap, :src_rect, :viewport, :visible, :x, :y, :z, :ox, :oy
  attr_accessor :zoom_x, :zoom_y, :angle, :wave_amp, :wave_length
  attr_accessor :wave_speed, :wave_phase, :mirror, :bush_depth, :bush_opacity
  attr_accessor :opacity, :blend_type, :color, :tone
  def src_rect=(src_rect); @src_rect.set(src_rect); end
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
class Viewport
  def initialize(*args)
    if args.length == 4 then
      x, y, width, height = *args
      @rect = Rect.new(x, y, width, height)
    elsif args.length == 1 then
      rect, = *args
      @rect = Rect.new
      @rect.set(rect)
    elsif args.length == 0 then
      @rect = Rect.new(0, 0, Graphics.width, Graphics.height)
    else
      raise ArgumentError.new("wrong number of arguments (#{args.length} for 0 or 1 or 4)")
    end
    @visible = true
    @z = 0
    @ox = 0
    @oy = 0
    @color = Color.new
    @tone = Tone.new

    @disposed = false
    nil
  end
  def dispose; @disposed = true; nil; end
  def disposed?; @disposed; end
  def flash(color, duration); end
  def update; end
  attr_accessor :rect, :visible, :z, :ox, :oy, :color, :tone
  def rect=(rect); @rect.set(rect); end
  def color=(color); @color.set(color); end
  def tone=(tone); @tone.set(tone); end
end
class Window
  def initialize(*args)
    if args.length == 4 then
      x, y, width, height = *args
      @x = x
      @y = y
      @width = width
      @height = height
    elsif args.length == 0 then
      @x = 0
      @y = y
      @width = 0
      @height = 0
    else
      raise ArgumentError.new("wrong number of arguments (#{args.length} for 0 or 4)")
    end
    @windowskin = Bitmap.new(1,1)
    @contents = Bitmap.new(1,1)
    @cursor_rect = Rect.new
    @viewport = nil
    @active = true
    @visible = true
    @arrows_visible = true
    @pause = false
    @z = 100
    @ox = 0
    @oy = 0
    @padding = 12
    @padding_bottom = 12
    @opacity = 255
    @back_opacity = 192
    @contents_opacity = 255
    @openness = 255
    @tone = Tone.new

    @disposed = false
    nil
  end
  def dispose; @disposed = true; nil; end
  def disposed?; @disposed; end
  def update; end
  def move(x, y, width, height)
    @x = x
    @y = y
    @width = width
    @height = height
  end
  def open?; openness == 255; end
  def close?; openness == 0; end
  attr_accessor :windowskin, :contents, :cursor_rect, :viewport, :active
  attr_accessor :visible, :arrows_visible, :pause, :x, :y, :width, :height
  attr_accessor :z, :ox, :oy, :padding, :padding_bottom, :opacity
  attr_accessor :back_opacity, :contents_opacity, :openness, :tone
  def cursor_rect=(cursor_rect); @cursor_rect.set(cursor_rect); end
  def tone=(tone); @tone.set(tone); end
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

def load_data(path)
  path = path.gsub("\\", "/")
  File.open(path, "r") do|file|
    Marshal.load(file)
  end
end
def rgss_main
  yield
rescue RGSSReset
  retry
end
File.open("Data/Scripts.rvdata2", "r") do|file|
  Marshal.load(file).each do|num,title,script|
    s = Zlib::Inflate::inflate(script)
    s.force_encoding("utf-8")
    eval(s, binding, title)
  end
end
