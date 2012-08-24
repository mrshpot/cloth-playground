angle = 0
rotation_r = 0.5
angle_speed = 1.5

function init()
   local s
   print("Hello from Lua")
   s = Sphere.new(0.0, -0.5, 0.1, 0.4)
   spheres:append(s)
end

function update(dt)
   local s = spheres:get(1)
   angle = angle + (angle_speed * dt)
   s.x = math.sin(angle) * rotation_r
   s.z = math.cos(angle) * rotation_r
end
