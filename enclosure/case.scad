$fn = 50;

w = 50;
h = 80;
r = 5;

rim_h = 10;

head_d = 9;
body_d = 5;

// y coordinate of upper screwholes
yy_up = h / 2 - 5 - 3 - head_d - 5;
yy_down = -h / 2 + 2;
xx_left = -w / 2 + 2;
xx_right = w / 2 - 2;

module base_shape(w, h, r)
{
    minkowski()
    {
        square([w,h], center = true);
        circle(r);
    }
}

module base_2d()
{
    difference()
    {
        base_shape(w, h, r);
        translate([0, h/2 - 5, 0]) union()
        {
            hull()
            {
                circle(d = body_d);
                translate([0, -body_d*1.5, 0]) circle(d = body_d);
            }
            translate([0, -body_d*1.5, 0]) circle(d = head_d);
        }
    }
}

module rim_2d()
{
    difference()
    {
        base_shape(w, h, r);
        base_shape(w, h, r - 1);
    }
}

module screwhole()
{
    difference()
    {
        cylinder(h = 5, d1 = 8, d2 = 4);
        cylinder(h = 6, d = 1);
    }
}

module arch(size)
{
    rotate([90, 0, 90]) linear_extrude(2, center = true) difference()
    {
        square(size);
        intersection()
        {
            circle(r = size);
            square(size + 1);
        }
    }
}

module rimhole(size, r)
{
    translate([0, 0, rim_h/2]) cube([2, size, rim_h], center = true);
    translate([0, -r - size/2, rim_h - r]) arch(r);
    translate([0, r + size/2, rim_h - r]) rotate([0, 0, 180]) arch(r);
}

module rim()
{
    difference()
    {
        linear_extrude(rim_h) rim_2d();
        translate([-w/2 - r, yy_up - 10, 0])
            rimhole(9, 5);
         translate([w/2 + r, yy_up - 10, 0])
            rimhole(9, 5);
         translate([0, -h/2 - r + 0.5, 0])
         rotate([0, 0, 90])
            rimhole(15, 5);
    }
}

linear_extrude(1) base_2d();

translate([xx_left, yy_down, 0]) screwhole();
translate([xx_right, yy_down, 0]) screwhole();
translate([xx_left, yy_up, 0]) screwhole();
translate([xx_right, yy_up, 0]) screwhole();

rim();

echo("Horizontal screw distance: ", xx_right - xx_left);
echo("Vertical screw distance: ", yy_up - yy_down);