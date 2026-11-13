-- Excerpt of ObjectCreation.lua (polygon vertices and instance scale), recovered from the supplied Classic APK; read only.
  if L67_2 == "polygon" then
    if L58_2 ~= "" and L58_2 ~= nil then
      L67_2 = res
      L67_2 = L67_2.getSpriteBounds
      L68_2 = ""
      L69_2 = L58_2
      L67_2, L68_2 = L67_2(L68_2, L69_2)
      L64_2 = L68_2
      L63_2 = L67_2
      if L10_2 == 0 then
        L61_2 = 1
        L62_2 = 1
      end
      L67_2 = gamelua
      L67_2 = L67_2.physicsScale
      L67_2 = L63_2 * L67_2
      L63_2 = L67_2 * L61_2
      L67_2 = gamelua
      L67_2 = L67_2.physicsScale
      L67_2 = L64_2 * L67_2
      L64_2 = L67_2 * L62_2
      L67_2 = res
      L67_2 = L67_2.getSpritePivot
      L68_2 = ""
      L69_2 = L58_2
      L67_2, L68_2 = L67_2(L68_2, L69_2)
      L54_2 = L68_2
      L53_2 = L67_2
    else
      L63_2 = L7_2.width
      L64_2 = L7_2.height
      L67_2 = res
      L67_2 = L67_2.getSpritePivot
      L68_2 = ""
      L69_2 = L58_2
      L67_2, L68_2 = L67_2(L68_2, L69_2)
      L54_2 = L68_2
      L53_2 = L67_2
    end
    if L17_2 == nil then
      L17_2 = true
    end
    L67_2 = L63_2
    L66_2 = L64_2
    L65_2 = L67_2
    L67_2 = A5_2.scaleY
    if L67_2 then
      L67_2 = A5_2.scaleY
      if L67_2 ~= 0 then
        L64_2 = L64_2 * L52_2
      end
    end
    L67_2 = A5_2.scaleX
    if L67_2 then
      L67_2 = A5_2.scaleX
      if L67_2 ~= 0 then
        L63_2 = L63_2 * L51_2
      end
    end
    L67_2 = {}
    L68_2 = L7_2.vertices
    if L68_2 ~= nil then
      L68_2 = 1
      L69_2 = L7_2.vertices
      L69_2 = #L69_2
      L70_2 = 1
      for L71_2 = L68_2, L69_2, L70_2 do
        L72_2 = L7_2.vertices
        L72_2 = L72_2[L71_2]
        L73_2 = table
        L73_2 = L73_2.insert
        L74_2 = L67_2
        L75_2 = {}
        L76_2 = L72_2.x
        L76_2 = L76_2 * L63_2
        L77_2 = L63_2 * 0.5
        L76_2 = L76_2 - L77_2
        L75_2.x = L76_2
        L76_2 = L72_2.y
        L76_2 = L76_2 * L64_2
        L77_2 = L64_2 * 0.5
        L76_2 = L76_2 - L77_2
        L75_2.y = L76_2
        L73_2(L74_2, L75_2)
      end
    end
    L68_2 = gamelua
    L68_2 = L68_2.createPolygon
    L69_2 = L6_2
    L70_2 = L58_2
    L71_2 = A3_2
    L72_2 = A4_2
    L73_2 = L67_2
    L74_2 = L10_2
    L75_2 = L11_2
    L76_2 = L12_2
    L77_2 = L17_2
