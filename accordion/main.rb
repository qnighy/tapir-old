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
class RGSSError < StandardError; end

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
module Input
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
  GC.start
  retry
end

load_data("Data/Scripts.rvdata2").each do|num,title,script|
  s = Zlib::Inflate::inflate(script)
  s.force_encoding("utf-8")
  eval(s, binding, title)
end
