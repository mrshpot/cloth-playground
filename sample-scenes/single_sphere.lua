function init()
   s = Sphere.new(0.0, 0.4, 0.0, 0.4)
   spheres:append(s)
   
   p = Plane.new(0.0, 1.0, 0.0, 0.0)
   p:from_triangle({0.0, -0.4, 0.0},
   				   {0.0, -0.9, 1.0},
   				   {1.0, -0.9, 0.0})
   planes:append(p)
end

function update(dt)
end
