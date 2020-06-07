
player.idx_auras = math.floor(time / 10.0)

player.idx_lwing = math.floor(time / 16.0)
player.idx_rwing = math.floor(time / 16.0)

player.idx_cpits = math.floor(time / 13.0)

player:update()

if not (keys[UP] and keys[DOWN]) then
	if keys[UP]    then
		player:move( 0, -1)
	end
	
	if keys[DOWN]  then
		player:move( 0,  1)
	end
end

if not (keys[LEFT] and keys[RIGHT]) then
	if keys[LEFT]  then
		player:move(-1,  0)
	end
	
	if keys[RIGHT] then
		player:move( 1,  0)
	end
end