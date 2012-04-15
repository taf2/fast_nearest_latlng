$:.unshift '.'
require 'fastloc'

10.times do

floc = FastLoc.new(:unit => :miles)
puts floc.unit
floc.unit = :kilometers
puts floc.unit
floc.unit = :nautical_miles
puts floc.unit
floc.unit = :miles
puts floc.unit

floc.add(39.0566, -76.5352, 1)
floc.add(39.07, -76.5398, 2)
floc.add(39.0566, -76.5352, 3)
floc.add(39.07, -76.5398, 4)
floc.add(39.0566, -76.5352, 5)
floc.add(39.07, -76.5398, 6)
floc.add(39.0566, -76.5352, 7)
floc.add(39.07, -76.5398, 8)
floc.add(39.0566, -76.5352, 9)
floc.add(39.07, -76.5398, 10)
floc.add(39.0566, -76.5352, 11)
floc.add(39.07, -76.5398, 12)
floc.add(39.0566, -76.5352, 13)
floc.add(39.07, -76.5398, 14)
floc.add(39.0566, -76.5352, 15)
floc.add(39.07, -76.5398, 16)
floc.add(39.0566, -76.5352, 17)
floc.add(39.07, -76.5398, 18)
floc.add(39.0566, -76.5352, 19)
floc.add(39.07, -76.5398, 20)

#puts floc.locations.inspect
t = Time.now
n = 1000
n.times do
  floc.nearest(39.0735, -76.5654)
end
dur = Time.now - t
rate = n / dur
puts "processed: #{dur} seconds #{rate} per second with 20 possible"

floc.reset
floc=nil
end
GC.start
