'use strict';
/* NEONDOSE 2 — all processing is local, rule-assisted style design and Web Audio DSP.
   Presets are effect recipes, not licensed voices, voice cloning or AI singing. */
const $=id=>document.getElementById(id);
const PARAMS=[
 {key:'DRIVE',min:0,max:100,initial:10,unit:'%'},
 {key:'TONE',min:0,max:100,initial:47,unit:'%'},
 {key:'AIR',min:0,max:100,initial:38,unit:'%'},
 {key:'COMP',min:0,max:100,initial:39,unit:'%'},
 {key:'SPACE',min:0,max:100,initial:15,unit:'%'},
 {key:'DELAY',min:0,max:100,initial:12,unit:'%'},
 {key:'MIX',min:0,max:100,initial:100,unit:'%'},
 {key:'OUTPUT',min:-18,max:6,initial:-3,unit:' dB'}
];
const defaultValues=Object.fromEntries(PARAMS.map(p=>[p.key,p.initial]));
let values={...defaultValues}, currentStyle=null, currentCategory='All', favoritesOnly=false, promptBase=null, promptInput='', engine=null, mediaSource=null, micSource=null, micStream=null, sourceKind='none', objectUrl=null, bufferForExport=null, currentFileName='';
const genres=['Emo / Indie','Trap / Rap','Metal / Rock','Pop / R&B','Electronic','Lo-fi / Alt','Experimental'];
// name, category, descriptor, inspiration/context tags, [drive, tone, air, compression, space, delay, mix, output]
const cores=[
 ['Bedroom Emo Rap','Emo / Indie','Intimate tape-colored verses, shy melodic hooks','powfu soft sad diary emo lo-fi bedroom',[8,44,35,43,15,10,100,-3]],
 ['Heartbreak Pop Rap','Emo / Indie','Youthful close vocals with melodic pop gloss','the kid laroi emo pop rap heartbreak melodic',[12,62,55,56,26,19,100,-4]],
 ['Rainy Window','Emo / Indie','Muted late-night melancholy and damp-room ambience','sad sleepy soft acoustic lo-fi',[5,31,24,32,40,13,100,-3]],
 ['Acoustic Confession','Emo / Indie','Clear restrained singer-songwriter delivery','acoustic indie intimate delicate confession',[4,55,40,30,18,7,100,-3]],
 ['Indie Bedroom Tape','Emo / Indie','Dusty cassette warmth and inward-sounding vocal','bedroom indie current joys dusty vintage',[13,35,22,34,24,15,100,-3]],
 ['Fragile Falsetto','Emo / Indie','Soft, airy upper-register presence','falsetto vulnerable emotional whisper',[4,58,73,38,32,16,100,-3]],
 ['Nocturnal Melodic Rap','Trap / Rap','Warm low-mid body with reserved late-night echo','drake melodic rap nocturnal moody',[11,49,30,55,25,31,100,-4]],
 ['Psychedelic Trap','Trap / Rap','Smoky saturation and cavernous rhythmic repeats','travis scott psychedelic trap dreamy atmospheric',[26,41,47,53,40,48,100,-5]],
 ['Cloud Rap Drift','Trap / Rap','Hazy low-key vocal washed in delay','neb korusai underground cloud rap dreamy',[7,41,56,38,49,57,100,-4]],
 ['Drill Focus','Trap / Rap','Tight upfront low-mid vocal with limited ambience','drill hard rap punchy focused upfront',[22,50,33,71,8,9,100,-5]],
 ['Classic Hip-Hop','Trap / Rap','Punchy clean verses with short vintage ambience','old school hip hop golden era',[17,48,24,63,12,10,100,-4]],
 ['Melodic Trap Gloss','Trap / Rap','Airy compressed modern pop-trap vocal','melodic trap gloss commercial clean',[16,60,65,68,22,30,100,-5]],
 ['Vintage Doom','Metal / Rock','Dark vintage heavy-rock voice, restrained grime','black sabbath vintage doom metal ozzy style',[29,34,19,59,15,11,100,-5]],
 ['Soaring Heavy Metal','Metal / Rock','Upfront high-mid bite with clear sustained vocals','iron maiden classic heavy metal soaring power',[20,75,60,62,16,14,100,-5]],
 ['Modern Metalcore','Metal / Rock','Dense present rock vocal with controlled edge','metalcore modern heavy angry',[37,63,45,76,10,11,100,-6]],
 ['Grunge Tape','Metal / Rock','Weathered analog rock with a slightly rough body','grunge alt rock vintage emotional',[28,43,25,59,19,15,100,-5]],
 ['Power Ballad','Metal / Rock','Wide reverb behind a bright front vocal','hard rock power ballad epic soaring',[16,70,51,67,43,23,100,-5]],
 ['Post-Hardcore','Metal / Rock','Tense mid-forward rock with gritty edges','post hardcore emo scream aggressive',[33,60,39,73,18,12,100,-6]],
 ['Silky R&B','Pop / R&B','Close soft R&B shine with restrained dynamics','soft smooth soul rnb silky',[7,57,58,41,28,20,100,-3]],
 ['Dream Pop','Pop / R&B','Soft-focus spacious shimmer and glossy treble','dream pop ethereal floating',[6,57,73,42,61,37,100,-4]],
 ['Modern Pop Lead','Pop / R&B','Bright, radio-friendly vocal presence','modern pop radio catchy studio',[10,70,61,71,24,18,100,-4]],
 ['Warm Neo-Soul','Pop / R&B','Rounded harmonics with smooth subtle ambience','neo soul warm organic jazzy',[9,40,27,48,20,13,100,-3]],
 ['Dark Alt-Pop','Pop / R&B','Understated edge, dark presence and texture','alternative pop dark mysterious',[16,38,29,55,34,25,100,-4]],
 ['Hyperpop Edge','Pop / R&B','Bright synthetic brilliance with edgy saturation','hyperpop shiny digital experimental',[35,79,80,72,33,31,100,-6]],
 ['Vaporwave Haze','Electronic','Retro softened top end with diffuse tails','vaporwave retro dreamy nostalgic',[9,27,31,41,68,47,100,-4]],
 ['Synthwave Night','Electronic','Glossy neon ambience and steady presence','synthwave electronic retro 80s',[13,57,50,57,38,40,100,-4]],
 ['Ambient Electronica','Electronic','Weightless vocal tones and very long room tails','ambient electronic ethereal space',[4,47,69,35,83,30,100,-4]],
 ['UK Garage Vocal','Electronic','Rhythmic vocal brightness and compact repeats','garage house uk electronic club',[13,69,61,59,17,38,100,-4]],
 ['Industrial Pulse','Electronic','Metallic vocal color and raw tight saturation','industrial electronic mechanical aggressive',[42,52,36,71,24,22,100,-6]],
 ['Soft House Hook','Electronic','Polished dance-pop with lush room texture','house melodic light dance',[8,66,67,62,38,26,100,-4]],
 ['Dusty Cassette','Lo-fi / Alt','Worn warm tape character, low mids','lofi cassette tape vintage dusty',[17,26,20,41,16,8,100,-5]],
 ['Cloudy Underground','Lo-fi / Alt','Relaxed hazy bars with generous echoes','neb korusai cloudy experimental rap underground',[10,37,57,39,49,53,100,-5]],
 ['Shoegaze Distant','Lo-fi / Alt','Diffuse and submerged soft-rock vocals','shoegaze washed out dreamy distant',[13,40,44,47,78,40,100,-5]],
 ['Dark Bedroom Whisper','Lo-fi / Alt','Muted close vocal with barely-there saturation','whisper soft close dark late-night',[4,29,22,29,21,9,100,-3]],
 ['Lo-fi Jazz Booth','Lo-fi / Alt','Warmed low-volume croon, smoky short ambience','jazz lo-fi mellow analog',[8,36,19,36,25,9,100,-3]],
 ['Acoustic Dreamer','Lo-fi / Alt','Gentle intimate vocal with diffused high air','acoustic dreamy breathy folk',[3,52,64,32,48,15,100,-3]],
 ['Ghost Chorus','Experimental','Unnatural open room with dark spectral wash','ghost haunted dark ambient experimental',[11,27,61,49,89,65,100,-6]],
 ['Radio Transmission','Experimental','Band-limited vintage broadcast texture','radio telephone filtered vintage voice',[22,19,9,58,8,9,100,-5]],
 ['Digital Nebula','Experimental','Glassy airy saturation and sparkling repeats','digital space futuristic ethereal',[25,68,81,57,65,64,100,-6]],
 ['Underground Cyber','Experimental','Neon distorted underground vocal atmosphere','cyber glitch experimental neb korusai',[30,47,64,64,45,51,100,-6]],
 ['Dream Journal','Experimental','Slow-moving foggy vocal effect bed','dream vapor ambient sleepy surreal',[5,34,54,32,74,50,100,-4]],
 ['Intimate Minimalism','Experimental','Nearly dry close-up whisper with quiet dynamics','minimal intimate clear transparent',[0,53,45,28,2,0,100,-3]]
];
const variations=[['Core','Balanced production'],['Soft','Gentle, intimate, restrained'],['Wide','Airy with a spacious tail'],['Edge','Tighter and more saturated']];
const clamp=(x,a,b)=>Math.max(a,Math.min(b,x));
function adjustVariation(arr,v){let a=arr.slice();if(v===1){a[0]=Math.round(a[0]*.55);a[3]=Math.max(20,a[3]-12);a[4]=Math.round(a[4]*.55);a[5]=Math.round(a[5]*.55);a[7]=Math.min(a[7],-3)}else if(v===2){a[2]=Math.min(90,a[2]+13);a[4]=Math.min(86,a[4]+25);a[5]=Math.min(86,a[5]+19);a[7]=Math.min(a[7],-4)}else if(v===3){a[0]=Math.min(70,a[0]+17);a[3]=Math.min(88,a[3]+8);a[4]=Math.max(5,a[4]-9);a[7]=Math.min(a[7],-5)}return a}
const styles=cores.flatMap((c,i)=>variations.map(([suffix,detail],j)=>({id:i*4+j,coreIndex:i,variant:j,name:`${c[0]} · ${suffix}`,category:c[1],desc:j?detail:c[2],tags:`${c[0]} ${c[1]} ${c[2]} ${c[3]} ${detail}`.toLowerCase(),vals:adjustVariation(c[4],j)})));
let favs;try{favs=new Set(JSON.parse(localStorage.getItem('neondose-favorites')||'[]'))}catch(_){favs=new Set()}
function formatValue(key,v){return key==='OUTPUT'?`${v>0?'+':''}${v} dB`:`${v}%`}
function syncControls(){PARAMS.forEach((p,i)=>{const el=$('controls').children[i];el.querySelector('input').value=values[p.key];el.querySelector('output').textContent=formatValue(p.key,values[p.key]);el.querySelector('.dial').style.setProperty('--rotate',(values[p.key]-p.min)/(p.max-p.min)*250-125+'deg')});$('outputInfo').textContent='OUTPUT '+formatValue('OUTPUT',values.OUTPUT);applyDSP()}
function initControls(){PARAMS.forEach(p=>{let wrap=document.createElement('div');wrap.className='knob';wrap.innerHTML=`<label for="v${p.key}">${p.key}</label><div class="dial" aria-hidden="true"></div><input id="v${p.key}" aria-label="${p.key}" type="range" min="${p.min}" max="${p.max}" step="1" value="${p.initial}"><output for="v${p.key}"></output>`;const slide=wrap.querySelector('input');slide.oninput=()=>{values[p.key]=Number(slide.value);syncControls();$('lcdOutput').textContent='SAFE MASTER / ON';};$('controls').append(wrap)});syncControls()}
function styleLabel(s){return s.name.replace(' · ',' / ').toUpperCase()}
function setRecipe(style,{fromPrompt=false}={}){currentStyle=style;PARAMS.forEach((p,i)=>values[p.key]=style.vals[i]);syncControls();$('lcdStyle').textContent=styleLabel(style);$('lcdDetail').textContent=style.desc;$('lcdPrompt').textContent=fromPrompt?'PROMPT / APPLIED':'RECIPE / LOADED';$('libraryHint').textContent=style.name+' loaded';renderLibrary()}
function renderFilters(){const options=['All',...genres];$('filters').replaceChildren();for(const f of options){const b=document.createElement('button');b.className='chip'+(f===currentCategory?' active':'');b.textContent=f;b.onclick=()=>{currentCategory=f;renderFilters();renderLibrary()};$('filters').append(b)}}
function renderLibrary(){const q=$('search').value.toLowerCase().trim();const matches=styles.filter(s=>(currentCategory==='All'||s.category===currentCategory)&&(!favoritesOnly||favs.has(s.id))&&(!q||q.split(/\s+/).every(term=>s.tags.includes(term))));const root=$('voices');root.replaceChildren();for(const s of matches){const cell=document.createElement('div');cell.className='style-item'+(currentStyle?.id===s.id?' active':'');cell.setAttribute('role','listitem');const title=document.createElement('h3');title.textContent=s.name;const cat=document.createElement('span');cat.className='style-tag';cat.textContent=s.category.toUpperCase();const desc=document.createElement('p');desc.textContent=s.desc;const star=document.createElement('button');star.className='star'+(favs.has(s.id)?' active':'');star.textContent=favs.has(s.id)?'★':'☆';star.title=favs.has(s.id)?'Remove favorite':'Add favorite';star.setAttribute('aria-label',star.title);star.onclick=e=>{e.stopPropagation();if(favs.has(s.id))favs.delete(s.id);else favs.add(s.id);try{localStorage.setItem('neondose-favorites',JSON.stringify([...favs]))}catch(_){}renderLibrary()};cell.append(title,cat,desc,star);cell.onclick=()=>setRecipe(s);root.append(cell)}$('resultCount').textContent=`${matches.length} SHOWN`;$('libraryCount').textContent=`${styles.length} recipes`;if(!matches.length){const empty=document.createElement('p');empty.className='disclaimer';empty.textContent='No recipes matched. Try another genre or search term.';root.append(empty)}}
// Phrase-aware prompt-to-effects mapping: combines style similarity, reference matching,
// multiple descriptive clauses, negatives, degree qualifiers and explicit instructions.
// No remote API or generative vocal model is used.
const stopWords=new Set('make a an the this that it is i want please me my vocal vocals voice sound sounds singer singing with without like kind of style give create make for into some should feel and but also to be have as on just little bit very something less more not no than from get use want'.split(' '));
const referenceCues=[
 [/powfu|pow fu/,0],[/kid\s+laroi|laroi/,1],[/drake/,6],[/travis\s+scott/,7],[/neb|korusai/,8],[/black\s+sabbath|ozzy/,12],[/iron\s+maiden|bruce\s+dickinson/,13],[/current\s+joy/,4]
];
const synonymMap={
 'sad':['emo','melancholy','intimate','soft'], 'emotional':['emo','heartbreak','intimate'], 'heartbroken':['heartbreak','emo','sad'], 'diary':['bedroom','intimate','soft'],
 'whispery':['whisper','soft','intimate'], 'lofi':['lo-fi','dusty','bedroom','tape'], 'lo-fi':['lo-fi','dusty','bedroom','tape'],
 'gentle':['soft','intimate'], 'dreamy':['dream','cloud','ambient','hazy'], 'atmospheric':['ambient','spacious','dreamy'],
 'ethereal':['dream','air','ambient'], 'angry':['aggressive','hard','metal'], 'massive':['power','heavy','epic'],
 'bright':['clear','shine','gloss'], 'dark':['moody','night','doom'], 'vintage':['tape','analog','retro'],
 'rap':['rap','trap','hip-hop'], 'rock':['rock','metal','grunge'], 'melodic':['pop','melodic','dream'], 'fuzzy':['distorted','tape','grit'],
 'bedroom':['emo','indie','lo-fi'], 'underwater':['washed','shoegaze','ambient'], 'clean':['clear','transparent','minimal'],
 'distant':['dreamy','ambient','shoegaze'], 'near':['close','intimate'], 'close':['intimate','whisper','bedroom']
};
function normWords(txt){return txt.toLowerCase().replace(/lo[ -]?fi/g,'lofi').replace(/r\s*&\s*b/g,'rnb').replace(/hip[ -]?hop/g,'hiphop').replace(/[^a-z0-9 ]/g,' ').split(/\s+/).filter(Boolean)}
function matchCore(prompt){const lower=prompt.toLowerCase();const words=normWords(prompt).filter(x=>!stopWords.has(x));let expanded=new Set(words);for(const w of words)for(const s of synonymMap[w]||[])expanded.add(s.replace(/[^a-z]/g,''));let candidates=cores.map((core,i)=>{let label=(core[0]+' '+core[1]+' '+core[2]+' '+core[3]).toLowerCase();let coreWords=new Set(normWords(label));let score=0;for(const word of expanded){if(coreWords.has(word))score+=2;else if(word.length>4&&label.includes(word))score+=.55}for(const [regex,idx] of referenceCues)if(regex.test(lower)&&i===idx)score+=24;
if(/emo|heartbreak|sad/.test(lower)&&core[1]==='Emo / Indie')score+=2;
if(/rap|trap/.test(lower)&&core[1]==='Trap / Rap')score+=2;
if(/heavy|metal|rock/.test(lower)&&core[1]==='Metal / Rock')score+=2;
if(/cloud|underground/.test(lower)&&core[0].match(/Cloud|Underground/))score+=2;
if(/dream|hazy/.test(lower)&&/Dream|Cloud|Ambient|Haze/.test(core[0]))score+=2;
return{index:i,score}}).sort((a,b)=>b.score-a.score);
if(candidates[0].score<2.2) return {primary:0,secondary:null,score:0};
// Only blend a second recipe if the user explicitly mixes or combines multiple styles.
let secondary=null;if(/blend|combine|mix.*with|cross between|fusion|and.*(trap|emo|metal|pop|rnb|rock)/.test(lower)&&candidates[1].score>candidates[0].score*.60&&candidates[1].score>4)secondary=candidates[1].index;
return{primary:candidates[0].index,secondary,score:candidates[0].score};}
const aspects=[
 {name:'Intimate / close',regexp:/intimat|close[ -]?up|bedroom|soft[ -]?spoken|whisper|up[ -]?front|in your ear/g,delta:{DRIVE:-3,COMP:-6,SPACE:-8,DELAY:-7,TONE:-2}},
 {name:'Emotional / tender',regexp:/sad|heartbreak|emotional|vulnerab|delicate|fragile|gentle|mellow|melanchol/g,delta:{DRIVE:-5,COMP:-3,TONE:-4,SPACE:5}},
 {name:'Warm / analog',regexp:/warm|analog|tape|cassette|dusty|vintage|lo[ -]?fi|lofi/g,delta:{DRIVE:4,TONE:-9,AIR:-6}},
 {name:'Clear / clean',regexp:/clean|clear|transparent|polish|crisp|pristine|natural/g,delta:{DRIVE:-7,TONE:7,AIR:5,DELAY:-3}},
 {name:'Dark / muted',regexp:/dark|moody|muted|dull|low[ -]?key|nocturnal/g,delta:{TONE:-11,AIR:-10}},
 {name:'Bright / airy',regexp:/airy|breathy|bright|shimmer|sparkle|silky|gloss/g,delta:{TONE:6,AIR:13}},
 {name:'Grit / saturation',regexp:/grit|gritty|drive|saturat|distort|fuzz|rasp|overdrive|crunch/g,delta:{DRIVE:13,TONE:-3,COMP:3}},
 {name:'Heavy / aggressive',regexp:/aggress|growl|heavy|hard[ -]?hitting|scream|powerful|angry|raw/g,delta:{DRIVE:12,COMP:11,TONE:5}},
 {name:'Dreamy / spacious',regexp:/dreamy|space|spacious|ambient|washed|huge|cavern|floating|distant|ethereal/g,delta:{SPACE:15,DELAY:11,AIR:4}},
 {name:'Reverb / room',regexp:/reverb|room|hall|plate|lush|tail|wet/g,delta:{SPACE:16}},
 {name:'Echo / delay',regexp:/echo|delay|repeat|slapback/g,delta:{DELAY:17}},
 {name:'Punch / compression',regexp:/punch|tight|compressed|compression|steady|forward|dense/g,delta:{COMP:12,SPACE:-6,DELAY:-7}},
 {name:'Soft dynamics',regexp:/dynamic|uncompress|open|restrained|subtle/g,delta:{COMP:-8,DRIVE:-4}}
];
const directions={DRIVE:/driv|distort|saturat|grit|crunch|fuzz|rasp/,TONE:/ton|bright|dark|treble|mids|warm/,AIR:/air|breath|shimmer|high|sibilan/,COMP:/compress|comp|dynamic/,SPACE:/reverb|room|space|ambien|wet|hall/,DELAY:/delay|echo|repeat/,MIX:/\bmix\b|dry[ -]?wet/,OUTPUT:/volume|output|loud|level|gain/};
function nearbyIntensifier(before){if(/(?:super|very|extremely|really|lots of|big|much)\s*$/.test(before))return 1.6;if(/(?:slightly|a little|touch of|hint of|subtle|gentle|barely|light)\s*$/.test(before))return .48;return 1}
function buildPrompt(prompt){const lower=prompt.toLowerCase().replace(/\s+/g,' ').trim();const match=matchCore(lower);let arr=cores[match.primary][4].slice();let explanation=[`Starting point: ${cores[match.primary][0]}`];if(match.secondary!==null){const b=cores[match.secondary][4];arr=arr.map((v,i)=>Math.round(v*.65+b[i]*.35));explanation.push(`Blended with ${cores[match.secondary][0]}`)}let v=Object.fromEntries(PARAMS.map((p,i)=>[p.key,arr[i]]));let detected=[];
const segments=lower.split(/[,;.!?]+|\bbut\b|\bwhile\b/).map(x=>x.trim()).filter(Boolean);
const negated=new Set();const targeted=[];
for(const seg of segments){
 // Requests 'no [harsh] distortion', 'less drive', 'without echo', etc.
 const negRx=/(?:\bno\b|\bwithout\b|\bavoid\b|\bnot\b|\bless\b|\breduce\b|\blower\b|\bcut\b|\bremove\b)\s+(?:(?:too|much|any|harsh|heavy|extra|the|some|of|a|little|bit|more|very)\s+){0,4}([a-z][a-z-]*)/g;
 for(let m; (m=negRx.exec(seg));){const term=m[1];for(const [key,rx] of Object.entries(directions)){rx.lastIndex=0;if(rx.test(term)){const absolute=/\b(no|without|avoid|remove|not)\b/.test(m[0]);v[key]=absolute?(key==='DRIVE'||key==='DELAY'||key==='SPACE'?0:key==='OUTPUT'?-5:Math.max(12,v[key]-18)):clamp(v[key]-(key==='OUTPUT'?3:20),PARAMS.find(p=>p.key===key).min,PARAMS.find(p=>p.key===key).max);negated.add(key);targeted.push(`${absolute?'Without':'Less'} ${key.toLowerCase()}`)}}
 }
}
for(const a of aspects){let found=false;let matches=[];for(const seg of segments){a.regexp.lastIndex=0;for(let m; (m=a.regexp.exec(seg));){let pre=seg.substring(Math.max(0,m.index-28),m.index);if(/(?:\bno\b|\bwithout\b|\bavoid\b|\bless\b|\breduce\b|\bnot\b)\s+(?:\w+\s+){0,3}$/.test(pre))continue;const strength=nearbyIntensifier(pre);matches.push(strength)}}if(matches.length){const scale=Math.min(2.2,matches.reduce((a,b)=>a+b,0));for(const [key,n] of Object.entries(a.delta)){if(!negated.has(key))v[key]+=Math.round(n*scale)}found=true}if(found)detected.push(a.name)}
// Recognize direct numerical instructions for drive/output, but never imply neural autotune is available.
const driveMatch=lower.match(/(?:set\s+)?drive\s*(?:to|at|=)\s*(\d{1,3})\s*%?/);if(driveMatch){v.DRIVE=clamp(+driveMatch[1],0,100);targeted.push('Drive set explicitly')}
const outMatch=lower.match(/(?:set\s+)?(?:output|volume|gain)\s*(?:to|at|=)\s*([+-]?\d{1,2})\s*(?:db|decibels)/);if(outMatch){v.OUTPUT=clamp(+outMatch[1],-18,6);targeted.push('Output set explicitly')}
const unsupported=[];if(/auto[ -]?tune|pitch correct|tuned to|key of |sing in |sing like|clone|sound exactly|identical voice|change.*voice|voice model|voice conversion|ai voice/.test(lower))unsupported.push('AI voice identity conversion / autotune is not part of this prototype');
if(/harmoni[sz]|choir|double track|formant/.test(lower))unsupported.push('Harmonizer, voice doubling and formant conversion require additional DSP');
for(const p of PARAMS)v[p.key]=Math.round(clamp(v[p.key],p.min,p.max));
// Safety: aggressive saturation gets additional downward output trim.
if(v.DRIVE>44)v.OUTPUT=Math.min(v.OUTPUT,-5);
return{vals:v,base:match.primary,secondary:match.secondary,descriptors:[...new Set([...detected,...targeted])],unsupported,explanation,found:match.score>0||detected.length>0||targeted.length>0}}
function showPromptResult(res){let el=$('promptResult');el.replaceChildren();const title=document.createElement('strong');title.textContent=res.found?`Applied: ${cores[res.base][0]}`:'Using gentle bedroom vocal as a starting point';const body=document.createElement('span');body.textContent=(res.descriptors.length?'Mix directions: '+res.descriptors.join(' · '):'Your prompt did not include specific audio directions; add mood, production texture or effects.')+(res.unsupported.length?'  |  Not included: '+res.unsupported.join('; '):'');el.append(title,body)}
function customStyle(vals,name,desc){return{id:'custom',name,category:'Custom',desc,tags:'custom',vals:PARAMS.map(p=>vals[p.key])}}
function renderVariants(result){$('variations').hidden=false;$('variantList').replaceChildren();const options=[['Softer / closer',1],['More atmosphere',2],['More edge',3]];for(const [label,kind] of options){let trial=adjustVariation(PARAMS.map(p=>result.vals[p.key]),kind);const btn=document.createElement('button');btn.textContent=label;const detail=document.createElement('small');detail.textContent=kind===1?'Less grit and space':kind===2?'Airier with echo':'Extra textured bite';btn.append(detail);btn.onclick=()=>{setRecipe(customStyle(Object.fromEntries(PARAMS.map((p,i)=>[p.key,trial[i]])),`PROMPT · ${label}`,`Your description / ${label.toLowerCase()}`),{fromPrompt:true});$('promptResult').querySelector('strong').textContent='Variation active: '+label};$('variantList').append(btn)}}
function applyPrompt(){const raw=$('prompt').value.trim();if(!raw){$('promptResult').textContent='Type a sound description to build a custom vocal-production recipe.';return}let result=buildPrompt(raw);promptBase=result;promptInput=raw;setRecipe(customStyle(result.vals,'Custom prompt','Your personalized effects recipe'),{fromPrompt:true});showPromptResult(result);renderVariants(result)}
function initPrompt(){$('apply').onclick=applyPrompt;$('prompt').addEventListener('keydown',e=>{if((e.ctrlKey||e.metaKey)&&e.key==='Enter')applyPrompt()});$('random').onclick=()=>{const ideas=[
'Gentle intimate emo rap like Powfu, quiet melancholy bedroom recording, little saturation, short room, less echo.',
'An airy shoegaze vocal that sounds distant and dreamy, wide reverb, slight tape warmth, without harsh distortion.',
'Big clear power metal singer production inspired by classic British heavy metal, focused mids, controlled grit and little reverb.',
'Experimental cloud rap in the vein of Neb and Korusai, airy dreamy vocal, dark tape haze, moderate echo, less compression.',
'Dark nocturnal melodic trap, punchy steady level, subtle warm saturation, a bit of echo and smooth top end.',
'Nearly dry close acoustic vocal, warm and soft, no drive, no echo, no reverb, clear consonants.'
];$('prompt').value=ideas[Math.floor(Math.random()*ideas.length)];applyPrompt()};$('reset').onclick=()=>{$('prompt').value='';$('variations').hidden=true;$('promptResult').innerHTML='<strong>Reset complete.</strong><span>Use a recipe or type a new custom sound description.</span>';setRecipe(styles[0])};$('examples').addEventListener('click',e=>{const b=e.target.closest('button[data-example]');if(b){$('prompt').value=b.dataset.example;applyPrompt()}})}
// Web Audio effects graph shared by live playback and offline export.
// Values are automated smoothly, with separate original/processed paths and master limiting.
function dbToGain(db){return Math.pow(10,db/20)}
function makeImpulse(audioCtx){const sampleRate=audioCtx.sampleRate,len=Math.round(sampleRate*1.75);const buf=audioCtx.createBuffer(2,len,sampleRate);let seed=7243349;const random=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed/4294967296};for(let c=0;c<2;c++){const a=buf.getChannelData(c);let smooth=0;for(let i=0;i<len;i++){const t=i/sampleRate;const env=Math.pow(1-i/len,2.2);smooth=smooth*.30+(random()*2-1)*.70;a[i]=smooth*env*.42}}return buf}
function saturationCurve(drive){let curve=new Float32Array(4096);const amount=drive/100,k=1+amount*5.5,normalizer=Math.pow(k,.80);for(let i=0;i<curve.length;i++){const x=i/(curve.length-1)*2-1;curve[i]=Math.tanh(x*k)/normalizer}return curve}
function makeGraph(audioCtx,output){const g={ctx:audioCtx};
g.input=audioCtx.createGain();g.dry=audioCtx.createGain();g.highpass=audioCtx.createBiquadFilter();g.highpass.type='highpass';g.highpass.frequency.value=65;g.tone=audioCtx.createBiquadFilter();g.tone.type='lowpass';g.air=audioCtx.createBiquadFilter();g.air.type='highshelf';g.air.frequency.value=5200;g.comp=audioCtx.createDynamicsCompressor();g.comp.knee.value=10;g.comp.attack.value=.010;g.comp.release.value=.155;
g.clean=audioCtx.createGain();g.saturator=audioCtx.createWaveShaper();g.saturator.oversample='4x';g.satGain=audioCtx.createGain();g.stage=audioCtx.createGain();g.effectMix=audioCtx.createGain();g.delay=audioCtx.createDelay(2.0);g.feedback=audioCtx.createGain();g.delayWet=audioCtx.createGain();g.delayFilter=audioCtx.createBiquadFilter();g.delayFilter.type='lowpass';g.delayFilter.frequency.value=3600;
g.reverb=audioCtx.createConvolver();g.reverb.buffer=makeImpulse(audioCtx);g.reverbWet=audioCtx.createGain();g.bus=audioCtx.createGain();g.limiter=audioCtx.createDynamicsCompressor();g.limiter.threshold.value=-5;g.limiter.knee.value=0;g.limiter.ratio.value=20;g.limiter.attack.value=.002;g.limiter.release.value=.12;g.output=audioCtx.createGain();g.analyser=audioCtx.createAnalyser();g.analyser.fftSize=1024;
g.input.connect(g.dry);g.dry.connect(g.bus);
g.input.connect(g.highpass);g.highpass.connect(g.tone);g.tone.connect(g.air);g.air.connect(g.comp);
g.comp.connect(g.clean);g.clean.connect(g.stage);
g.comp.connect(g.saturator);g.saturator.connect(g.satGain);g.satGain.connect(g.stage);
g.stage.connect(g.effectMix);g.effectMix.connect(g.bus);
g.stage.connect(g.delay);g.delay.connect(g.delayFilter);g.delayFilter.connect(g.delayWet);g.delayWet.connect(g.bus);
g.delay.connect(g.feedback);g.feedback.connect(g.delay);
g.stage.connect(g.reverb);g.reverb.connect(g.reverbWet);g.reverbWet.connect(g.bus);
g.safetyTrim=audioCtx.createGain();g.safetyTrim.gain.value=.48;
g.bus.connect(g.limiter);g.limiter.connect(g.safetyTrim);g.safetyTrim.connect(g.output);g.output.connect(g.analyser);g.analyser.connect(output);
return g}
function setGraph(g,v,immediate=false){if(!g)return;const now=g.ctx.currentTime, set=(node,val)=>{const param=node.gain||node;if(immediate)param.setValueAtTime(val,now);else{param.cancelScheduledValues(now);param.setTargetAtTime(val,now,.026)}};
set(g.dry,Math.cos(v.MIX/100*Math.PI/2));set(g.effectMix,Math.sin(v.MIX/100*Math.PI/2));
// High drive fades between an unchanged dynamics path and compressed soft clipping.
const satBlend=v.DRIVE/100*.8;set(g.clean,1-satBlend);set(g.satGain,satBlend);g.saturator.curve=saturationCurve(v.DRIVE);
g.highpass.frequency.setTargetAtTime(65,now,.02);
g.tone.frequency.setTargetAtTime(1250+Math.pow(v.TONE/100,1.55)*15750,now,.028);g.tone.Q.setTargetAtTime(.71,now,.02);
g.air.gain.setTargetAtTime((v.AIR-50)*.17,now,.03);
g.comp.threshold.setTargetAtTime(-11-v.COMP*.22,now,.026);g.comp.ratio.setTargetAtTime(1.2+v.COMP*.062,now,.026);
g.delay.delayTime.setTargetAtTime(.1+v.DELAY*.0053,now,.03);set(g.feedback,.10+v.DELAY*.0032);
// Wet sends remain below the dry path rather than summing two full-volume signals.
const m=Math.sin(v.MIX/100*Math.PI/2);set(g.delayWet,Math.pow(v.DELAY/100,1.25)*.24*m);set(g.reverbWet,Math.pow(v.SPACE/100,1.3)*.32*m);
set(g.output,dbToGain(v.OUTPUT));}
function applyDSP(){if(engine)setGraph(engine,values);$('outputInfo').textContent='OUTPUT '+formatValue('OUTPUT',values.OUTPUT)}
const player=$('player');function setInfo(message){$('message').textContent=message}
function setupAudio(){if(engine)return engine;const Ctx=window.AudioContext||window.webkitAudioContext;if(!Ctx)throw Error('This browser does not support Web Audio.');const context=new Ctx({latencyHint:'interactive'});engine=makeGraph(context,context.destination);setGraph(engine,values,true);$('engineDot').classList.add('on');$('engineLabel').textContent='ENGINE READY';$('lcdClock').textContent='ENGINE ONLINE';meterLoop();return engine}
async function ensureAudio(){const e=setupAudio();await e.ctx.resume();$('engineLabel').textContent='ENGINE ACTIVE';return e}
async function detachInput(){player.pause();if(micSource){micSource.disconnect();micSource=null}if(micStream){micStream.getTracks().forEach(t=>t.stop());micStream=null}if(mediaSource){try{mediaSource.disconnect()}catch(_){}}sourceKind='none';$('inputSignal').classList.remove('on');$('inputLabel').textContent='NO INPUT';$('lcdInput').textContent='INPUT: NONE'}
function meterLoop(){if(!engine)return;let samples=new Float32Array(engine.analyser.fftSize);engine.analyser.getFloatTimeDomainData(samples);let peak=0;for(const x of samples)peak=Math.max(peak,Math.abs(x));$('level').style.width=Math.min(100,peak*100)+'%';$('peakLabel').textContent=peak<.00001?'-∞ dBFS':`${Math.round(20*Math.log10(peak))} dBFS`;requestAnimationFrame(meterLoop)}
$('start').onclick=async()=>{try{await ensureAudio();setInfo('Audio enabled. Choose an audio file or start live microphone processing.')}catch(e){setInfo('Audio could not start: '+e.message)}};
$('file').onchange=async e=>{const file=e.target.files[0];if(!file)return;try{const g=await ensureAudio();await detachInput();if(objectUrl)URL.revokeObjectURL(objectUrl);objectUrl=URL.createObjectURL(file);player.src=objectUrl;player.load();currentFileName=file.name;bufferForExport=file;
if(!mediaSource)mediaSource=g.ctx.createMediaElementSource(player);mediaSource.connect(g.input);sourceKind='file';$('lcdInput').textContent='FILE: '+file.name.slice(0,23).toUpperCase();$('inputSignal').classList.add('on');$('inputLabel').textContent='FILE LOADED';$('engineLabel').textContent='FILE MONITORING';await player.play();setInfo('File playing with live effects. Adjust the eight dials or type a style prompt.')}catch(err){setInfo('File playback needs interaction: '+err.message)}};
$('mic').onclick=async()=>{try{if(!navigator.mediaDevices?.getUserMedia)throw Error('Microphone needs a secure HTTPS or localhost page.');const g=await ensureAudio();await detachInput();micStream=await navigator.mediaDevices.getUserMedia({audio:{echoCancellation:false,noiseSuppression:false,autoGainControl:false}});micSource=g.ctx.createMediaStreamSource(micStream);micSource.connect(g.input);sourceKind='mic';$('lcdInput').textContent='INPUT: LIVE MICROPHONE';$('engineLabel').textContent='MIC LIVE';$('inputSignal').classList.add('on');$('inputLabel').textContent='MIC ACTIVE';setInfo('LIVE MICROPHONE: real-time effects are active. Wear headphones to prevent speaker feedback.')}catch(e){setInfo('Microphone unavailable: '+e.message)}};
$('stop').onclick=async()=>{await detachInput();$('engineLabel').textContent=engine?'ENGINE READY':'ENGINE OFFLINE';setInfo('Input stopped. Effects remain available when audio is restarted.')};
player.addEventListener('play',async()=>{if(sourceKind==='file'&&engine&&engine.ctx.state!=='running')await engine.ctx.resume()});
function encodeStereoWav(buffer){const sampleRate=buffer.sampleRate,n=buffer.length,channels=2,bytes=44+n*channels*2,ab=new ArrayBuffer(bytes),dv=new DataView(ab);const write=(offset,s)=>{for(let i=0;i<s.length;i++)dv.setUint8(offset+i,s.charCodeAt(i))};write(0,'RIFF');dv.setUint32(4,bytes-8,true);write(8,'WAVE');write(12,'fmt ');dv.setUint32(16,16,true);dv.setUint16(20,1,true);dv.setUint16(22,channels,true);dv.setUint32(24,sampleRate,true);dv.setUint32(28,sampleRate*channels*2,true);dv.setUint16(32,channels*2,true);dv.setUint16(34,16,true);write(36,'data');dv.setUint32(40,bytes-44,true);
const L=buffer.getChannelData(0),R=buffer.getChannelData(Math.min(1,buffer.numberOfChannels-1));let peak=0;for(let i=0;i<n;i++)peak=Math.max(peak,Math.abs(L[i]),Math.abs(R[i]));const gain=peak>.985?.985/peak:1;for(let i=0;i<n;i++){dv.setInt16(44+i*4,Math.round(clamp(L[i]*gain,-1,1)*32767),true);dv.setInt16(46+i*4,Math.round(clamp(R[i]*gain,-1,1)*32767),true)}return new Blob([ab],{type:'audio/wav'})}
$('export').onclick=async()=>{if(!bufferForExport){setInfo('Choose an audio file first. Live microphone capture is not exported.');return}const btn=$('export');btn.disabled=true;btn.textContent='Rendering…';try{let live=await ensureAudio();const raw=await bufferForExport.arrayBuffer();const audio=await live.ctx.decodeAudioData(raw.slice(0));const rate=audio.sampleRate;const maxSeconds=180;if(audio.duration>maxSeconds)throw Error('This prototype exports files up to 3 minutes. Choose a shorter clip.');const length=audio.length+Math.ceil(rate*2.25);const off=new OfflineAudioContext(2,length,rate);const graph=makeGraph(off,off.destination);setGraph(graph,values,true);const src=off.createBufferSource();src.buffer=audio;src.connect(graph.input);src.start(0);const result=await off.startRendering();const blob=encodeStereoWav(result);const u=URL.createObjectURL(blob);const a=document.createElement('a');a.href=u;a.download=(currentFileName||'vocal').replace(/\.[^.]+$/,'')+'-neondose.wav';document.body.append(a);a.click();a.remove();setTimeout(()=>URL.revokeObjectURL(u),12000);setInfo('Export complete: stereo 16-bit WAV with the current saturation, EQ, compression, reverb, delay and output level.')}catch(e){setInfo('Export failed: '+e.message)}finally{btn.disabled=false;btn.textContent='↓ Export WAV'}};
// Initial state: restrained output and a genuinely low-drive bedroom sound.
initControls();renderFilters();$('search').oninput=renderLibrary;$('favoritesOnly').onclick=()=>{favoritesOnly=!favoritesOnly;$('favoritesOnly').setAttribute('aria-pressed',String(favoritesOnly));renderLibrary()};initPrompt();setRecipe(styles[0]);
