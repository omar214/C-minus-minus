const express = require('express');
const bodyParser = require('body-parser');
const { exec, spawn } = require('child_process');
const fs = require('fs');

const app = express();

app.use(bodyParser.json());

const wslCommand = async (command) => {
	return new Promise((resolve, reject) => {
		const wslTerminal = spawn('wsl', [], { shell: true });

		let stdoutData = '';
		let stderrData = '';

		wslTerminal.stdout.on('data', (data) => {
			stdoutData += data;
		});

		wslTerminal.stderr.on('data', (data) => {
			stderrData += data;
		});

		wslTerminal.on('close', (code) => {
			if (code !== 0) {
				reject(
					new Error(`Failed to execute command: ${command}\n${stderrData}`),
				);
			} else {
				resolve(stdoutData);
			}
		});

		wslTerminal.stdin.write(command + '\n');
		wslTerminal.stdin.end();
	});
};

const getTestCasesDirectory = () => {
	let testCasesDirectory = __dirname;
	testCasesDirectory = testCasesDirectory.substring(
		0,
		testCasesDirectory.lastIndexOf('\\'),
	);
	testCasesDirectory = testCasesDirectory.substring(
		0,
		testCasesDirectory.lastIndexOf('\\'),
	);

	testCasesDirectory = testCasesDirectory + '\\test_cases';
	return testCasesDirectory;
};

app.post('/compile', async (req, res) => {
	let inputFile = req.body.inputFile;
	if (!inputFile) {
		return res.status(400).json({ error: 'Please provide input file .' });
	}
	// get the test_cases dir
	let testCasesDirectory = getTestCasesDirectory();

	// get the output dir
	let inputFilewithoutExtension = inputFile.split('.')[0];
	let outputDirectory = testCasesDirectory + '\\' + inputFilewithoutExtension;

	// convert to wsl path
	let wsl_outputDir =
		'\\mnt\\' +
		outputDirectory.charAt(0).toLowerCase() +
		outputDirectory.slice(2);

	let wsl_inputFile = wsl_outputDir + '\\' + inputFile;

	// fix path
	wsl_inputFile = wsl_inputFile.replace(/\\/g, '/');
	wsl_outputDir = wsl_outputDir.replace(/\\/g, '/');

	let command = `./compiler.exe "${wsl_inputFile}" "${wsl_outputDir}"`;
	console.log(command);

	try {
		// let command = `pwd`;
		const output = await wslCommand(command);
		console.log(`Output from WSL terminal: ${output}`);

		try {
			const quadruples = await fs.promises.readFile(
				`${outputDirectory}/quadruples.txt`,
				'utf8',
			);
			const symbolTable = await fs.promises.readFile(
				`${outputDirectory}/symbol_table.txt`,
				'utf8',
			);
			const errors = await fs.promises.readFile(
				`${outputDirectory}/errors.txt`,
				'utf8',
			);

			console.log(`--------- command executed successfully  --------- `);
			res.json({ quadruples, symbolTable, errors });
		} catch (error) {
			console.error(`Error reading files: ${error}`);
			return res.status(400).json({ error: error.message });
		}
	} catch (error) {
		console.error(`Error executing command in WSL terminal: ${error}`);
		res.json({ error: 'error' });
	}
});

// Start the server
const port = 3000;
app.listen(port, () => {
	console.log(`Server is listening on http://localhost:${port}`);
});
