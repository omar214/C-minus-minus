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

app.post('/compile', async (req, res) => {
	let outputDir = req.body.outputDir;
	let inputFile = req.body.inputFile;

	if (!inputFile || !outputDir) {
		return res
			.status(400)
			.json({ error: 'Please provide input file and output directory.' });
	}

	// convert to wsl path
	let wsl_inputFile =
		'\\mnt\\' + inputFile.charAt(0).toLowerCase() + inputFile.slice(2);
	let wsl_outputDir =
		'\\mnt\\' + outputDir.charAt(0).toLowerCase() + outputDir.slice(2);

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
				`${outputDir}/quadruples.txt`,
				'utf8',
			);
			const symbolTable = await fs.promises.readFile(
				`${outputDir}/symbol_table.txt`,
				'utf8',
			);
			const errors = await fs.promises.readFile(
				`${outputDir}/errors.txt`,
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
