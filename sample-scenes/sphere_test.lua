num_spheres = 7
angle = 0
sphere_r = 0.2
rotation_r = 0.6
angle_speed = 1.5

function init()
   local s
   print("Hello from Lua")

   for i = 1,num_spheres do
	  local s = Sphere.new(0.0, -0.5, 0.1, sphere_r)
	  spheres:append(s)
   end

   p = Plane.new(0.0, 1.0, 0.0, 0.0)
   p:from_triangle({0.0, -0.9, 0.0},
   				   {0.0, -0.9, 1.0},
   				   {1.0, -0.9, 0.0})
   planes:append(p)
end

function update(dt)
   angle = angle + (angle_speed * dt)
   for i = 1,num_spheres do
	  local s = spheres:get(i)
	  local s_angle = angle + (i * 2 * math.pi / num_spheres)
	  s.x = math.sin(s_angle) * rotation_r
	  s.z = math.cos(s_angle) * rotation_r
	  s.y = math.cos(s_angle * 5) * 0.3 - 0.5
   end
end
