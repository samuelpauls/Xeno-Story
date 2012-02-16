/* By Samuel Pauls */

uniform sampler2D framebuffer;
/* gl_TexCoord[0] ranges from [0, 1] for the framebuffer. */

uniform sampler2D light_map;
/* gl_TexCoord[1] ranges from almost [0, 1] for the light map. */

/* Light data is stored in a texture of GL_RGB with channels from [0, 1].
   First row of texels:
    - R specifies the compressed x position relative to the viewport.
    - G specifies the compressed y position relative to the viewport.
    - B specifies the compressed light radius.
   Second row of texels:
    - R specifies the light's red component.
    - G specifies the light's green component.
    - B specifies the light's blue component.
   Attempts were made to store light data with:
    - Arrays instead of textures, but slowdowns occured with <= 20 lights.
    - Integers in textures, but values couldn't be stored. */
uniform sampler2D lights_data;
uniform int light_count;
/* gl_TexCoord[2] ranges from [0, window width or height] for the lights. */

void main() {
  vec4 framebuffer_pixel = texture2D (framebuffer, gl_TexCoord[0].st);
  
  /* Get a light pixel from the light map for dimming the framebuffer pixel. */
  vec4 light_pixel = texture2D (light_map, gl_TexCoord[1].st);
  
  /* Increase the light pixel by the individual lights. */
  for (int l = 0; l < light_count; ++l) {
    /* Get light's index into the light data texture. */
    const float MAX_LIGHTS = 20.0F;
    float light_data_x = (0.5F + float(l)) / MAX_LIGHTS;
    
    vec2 light_pos;
    float light_radius;
    
    /* Get the light's position and radius. */
    vec4 light_data = texture2D (lights_data, vec2 (light_data_x, 0.25F));
    const float PADDING = 100.0F;
    const float VP_WIDTH = 800.0F;
    const float VP_HEIGHT = 581.0F;
    light_pos.x = light_data.x * (PADDING + VP_WIDTH + PADDING) - PADDING;
    light_pos.y = light_data.y * (PADDING + VP_HEIGHT + PADDING) - PADDING;
    light_radius = light_data.z * VP_WIDTH;
    
    /* Brighten the light pixel. */
    float dist_to_light = distance (light_pos, gl_TexCoord[2].xy);
    if (dist_to_light < light_radius) {
      float brightness = 1.0F - (dist_to_light / light_radius);
      vec4 light_color = texture2D (lights_data, vec2 (light_data_x, 0.75F));
      light_pixel += brightness * light_color;
    }
  }
  
  /* Clamp the light pixel so that we don't increase past 100% of the
     framebuffer pixel's original brightness. */
  light_pixel = clamp (light_pixel, 0.0F, 1.0F);
  
  gl_FragColor = framebuffer_pixel * light_pixel;
}